#include <fat16.h>

#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <kstdio.h>

#include <hdd.h>

/*
    clusters make up only the allocable space in the file system, e.i.:
    the reserved sectors and FAT tables are not divided in clusters
 */
typedef uint16_t fat_entry_t;
typedef fat_entry_t clu_addr_t;

static ata_sector_t _buf_sec;

static struct {
    unsigned int partition_num;
    unsigned int starting_clu_sec_num;  // where the "allocable" section begins
    unsigned int starting_fat_sec_num;  // where the 1st FAT begins
} _;

#define VOL_LABEL_LEN   11
#define TYPE_LEN        8
static struct {
    uint16_t    bytes_per_sector;
    uint8_t     sectors_per_cluster;
    uint16_t    reserved_sectors;
    uint8_t     num_fats;
    uint16_t    max_root_entries;
    uint16_t    sectors_per_fat;
    char        vol_label[VOL_LABEL_LEN+1];
    char        type[TYPE_LEN+1];
    uint16_t    signature;
} _vid;
#define CLUSTER_SIZE    (_vid.sectors_per_cluster * sizeof(ata_sector_t))

typedef struct {
    unsigned int    read_only   :   1;
    unsigned int    hidden      :   1;
    unsigned int    sys_file    :   1;
    unsigned int    vol_label   :   1;
    unsigned int    subdir      :   1;
    unsigned int    archive     :   1;
    unsigned int                :   2;
} __attribute__((packed)) attr_t;

#define FILE_BASENAME_LEN       8
#define FILE_EXTENSION_LEN      3
typedef struct {
    char        basename[FILE_BASENAME_LEN];
    char        extension[FILE_EXTENSION_LEN];
    attr_t      attr;
    uint8_t     reserved[10];
    uint16_t    time;
    uint16_t    date;
    uint16_t    starting_cluster;   // zero for an empty file
    uint32_t    size;   // in bytes
} __attribute__((packed)) dir_entry_t;


static
ata_sector_t *_read_sector(uint32_t sec_num)
{
    if (hdd_read_sector(_.partition_num, sec_num, &_buf_sec))
        return NULL;

    return &_buf_sec;
}

// returns the relative (to the FS) sector number of the first sector of
// the provided cluster number
static inline
uint32_t _clu2rsec(clu_addr_t clu_num)
{
    return clu_num * _vid.sectors_per_cluster + _.starting_clu_sec_num;
}

static
ata_sector_t *_read_cluster_sec(clu_addr_t clu_num, unsigned int sec_num)
{
    // clusters begin their numbering at 2, i.e.: there is no cluster #0 or #1
    if (clu_num < 2)
        return NULL;

    if (sec_num >= _vid.sectors_per_cluster)
        return NULL;

    clu_num -= 2;

    sec_num += _clu2rsec(clu_num);

    return _read_sector(sec_num);    
}

/*******************************************************************************
 functions for managing the FAT
*******************************************************************************/
#define FAT_ENTRIES_PER_SECTOR (sizeof(ata_sector_t)/sizeof(fat_entry_t))
#define STARTING_ROOT_CLU_NUM   2
#define ROOT_CLUSTERS   \
            (_vid.max_root_entries * sizeof(dir_entry_t) / CLUSTER_SIZE)

// only for clusters not belonging to the rootdir
static inline
clu_addr_t _fat_get_next_clu_num(clu_addr_t clu_num)
{
    uint32_t sec_num;
    uint16_t rentry;

    if (clu_num < STARTING_ROOT_CLU_NUM + ROOT_CLUSTERS)
        return 0xffff;

    clu_num -= ROOT_CLUSTERS;

    // sector number relative to the FAT containing the cluster address
    sec_num = clu_num / FAT_ENTRIES_PER_SECTOR;

    // sector number relative to the partition containing the cluster address
    sec_num += _.starting_fat_sec_num;

    // postion of the table entry relative to the sector
    rentry = clu_num % FAT_ENTRIES_PER_SECTOR;

    // read the sector containing the targeted FAT entry
    if (hdd_read_sector(_.partition_num, sec_num, &_buf_sec))
        return 0xffff;

    return *((clu_addr_t *) &_buf_sec + rentry);    
}

/*******************************************************************************
 functions for managing the root directory and directory entries
*******************************************************************************/
static inline
bool _dir_entry_is_unused(const dir_entry_t *de)
{
    return 0xe5 == *(uint8_t *) de;
}

// whether a directory entry belongs to  
static inline
bool _dir_entry_has_long_name(const dir_entry_t *de)
{
    if (de->attr.read_only && de->attr.hidden &&
        de->attr.sys_file && de->attr.vol_label)
    {
        return true;
    }
    
    return false;
}

static inline
bool _dir_entry_is_end(const dir_entry_t *de)
{
    return !(*(uint8_t *) de);
}

static inline
char _char2upcase(const char c)
{
    if (c >= 'a' && c <= 'z')
        return 'A' + (c - 'a');

    return c;
}

static inline
bool _char_is_valid(const char c)
{
    return  (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            ('_' == c || '-' == c);
}

static
bool _dir_entry_is_filename(const dir_entry_t *de, const char *filename)
{
    char basename[FILE_BASENAME_LEN];   // not really the "basename"
    char extension[FILE_EXTENSION_LEN];
    size_t i, j;

    // set the padding beforehand
    memset(basename,  ' ', FILE_BASENAME_LEN);
    memset(extension, ' ', FILE_EXTENSION_LEN);

    // copy basename    
    for (i = 0; filename[i]; i++) {
        if ('.' == filename[i]) {
            if (!i)
                return false;   // an empty basename was provided
            i++;
            break;
        }

        if (i >= FILE_BASENAME_LEN)
            return false;   // avoid buffer overrun

        if (!_char_is_valid(filename[i]))
            return false;
 
        basename[i] = _char2upcase(filename[i]);
    }

    // copy extension
    for (j = 0; filename[i]; i++, j++) {
        if (j >= FILE_EXTENSION_LEN)
            return false;   // avoid buffer overrun

        if (!_char_is_valid(filename[i]))
            return false;

        extension[j] = _char2upcase(filename[i]);
    }

    if (!strncmp(basename, de->basename, FILE_BASENAME_LEN) &&
        !strncmp(extension, de->extension, FILE_EXTENSION_LEN))
        return true;

    return false;
}

#define DIR_ENTRIES_PER_SECTOR  (sizeof(ata_sector_t) / sizeof(dir_entry_t))

#define SECTORS_PER_CLUSTER     (_vid.sectors_per_cluster)
static dir_entry_t *_rootdir_get_entry_num(unsigned int num)
{
    ata_sector_t *sec;
    unsigned int rsec, rentry, cluster_steps;
    clu_addr_t clu_addr;
    
    cluster_steps = num * sizeof(dir_entry_t) / CLUSTER_SIZE;

    if (cluster_steps >= ROOT_CLUSTERS)
        return NULL;

    clu_addr = STARTING_ROOT_CLU_NUM;
    clu_addr += cluster_steps;


    // sector where the entry is located in the cluster
    rsec = num * sizeof(dir_entry_t) / sizeof(*sec) % SECTORS_PER_CLUSTER;

    // entry relative to the sector
    rentry = num % DIR_ENTRIES_PER_SECTOR;

    // get the sector containing this directory entry
    if (!(sec = _read_cluster_sec(clu_addr, rsec)))
        return NULL;

    return (dir_entry_t *) sec + rentry;
}

#define MAX_ROOT_ENTRIES    (_vid.max_root_entries)
static dir_entry_t *_rootdir_find_entry(const char *filename)
{
    dir_entry_t *de;
    size_t i;

    for (i = 0; i < MAX_ROOT_ENTRIES; i++) {
        if (!(de = _rootdir_get_entry_num(i)))
            return NULL;

        if (_dir_entry_is_end(de))
            return NULL;

        if (_dir_entry_is_unused(de))
            continue;

        if (_dir_entry_has_long_name(de))
            continue;   // long names not supported

        if (_dir_entry_is_filename(de, filename))
            return de;
    }

    return NULL;    
}

static void _dir_display_entry(const dir_entry_t *de)
{
    char buf[16];

    strncpy(buf, de->basename, FILE_BASENAME_LEN);
    buf[FILE_BASENAME_LEN] = '\0';
    kprintf("name: <%s>", buf);

    strncpy(buf, de->extension, FILE_EXTENSION_LEN);
    buf[FILE_EXTENSION_LEN] = '\0';
    kprintf(", extension: <%s>\n", buf);
}

void fat16_display_root(void)
{
    int i;
    dir_entry_t *de;

    for (i = 0; ; i++) {
        if (!(de = _rootdir_get_entry_num(i)))
            return;

        if (_dir_entry_is_end(de)) {
            kprintf("end reached\n");
            return;
        }

        if (_dir_entry_is_unused(de))
            continue;

        if (_dir_entry_has_long_name(de))
            continue;

        kprintf("entry %03d ", i);
        _dir_display_entry(de);
    }
}

void fat16_display_dir_entry(const char *filename)
{
    dir_entry_t *de;

    if (!(de = _rootdir_find_entry(filename))) {
        kprintf("entry \"%s\" not found\n", filename);
        return;
    }

    _dir_display_entry(de);
}
/******************************************************************************/


// initialize the volume identifier
static int _init_vid(ata_sector_t *sec)
{
    uint8_t  *base;
    uint8_t  *p8;
    uint16_t *p16;

    base = (uint8_t *) sec;

    // bytes per sector
    p16 = (uint16_t *) (base + 0xb);
    _vid.bytes_per_sector = *p16;

    // sectors per cluster
    p8 = base + 0xd;
    _vid.sectors_per_cluster = *p8;

    // number of reserved sectors
    p16 = (uint16_t *) (base + 0xe);
    _vid.reserved_sectors = *p16;

    // number of FATs
    p8 = base + 0x10;
    _vid.num_fats = *p8;

    // max number of root dir entries
    p16 = (uint16_t *) (base + 0x11);
    _vid.max_root_entries = *p16;

    // sectors per FAT
    p16 = (uint16_t *) (base + 0x16);
    _vid.sectors_per_fat = *p16;

    p16 = (uint16_t *) (base + 0x1fe);
    _vid.signature = *p16;
   
    p8 = (base + 0x2b);
    strncpy(_vid.vol_label, (char *) p8, VOL_LABEL_LEN);
    _vid.vol_label[VOL_LABEL_LEN] = '\0';

    p8 = (base + 0x36);
    strncpy(_vid.type, (char *) p8, TYPE_LEN);
    _vid.type[TYPE_LEN] = '\0';


    return 0;
}

int fat16_init(unsigned int partition_num)
{
    ata_sector_t *sec;

    if (partition_num >= 4)
        return 1;

    _.partition_num = partition_num;

    // read the boot sector
    if (!(sec = _read_sector(0)))
        return 1;

    _init_vid(sec);

    // the volume ID sector is already included in the number of reserved
    // sectors
    _.starting_clu_sec_num = 
                _vid.reserved_sectors + _vid.num_fats * _vid.sectors_per_fat;

    _.starting_fat_sec_num = _vid.reserved_sectors;

    return 0;
}

void fat16_display_vid(void)
{
    kprintf("bytes per sector:       %8d\n", _vid.bytes_per_sector);
    kprintf("sectors per cluster:    %8d\n", _vid.sectors_per_cluster);
    kprintf("reserved sectors:       %8d\n", _vid.reserved_sectors);
    kprintf("num fats:               %8d\n", _vid.num_fats);
    kprintf("max roots entries:      %8d\n", _vid.max_root_entries);
    kprintf("sectors per FAT:        %8d\n", _vid.sectors_per_fat);
    kprintf("signature:              %8x\n", _vid.signature);
    kprintf("label: <%s>\n", _vid.vol_label);
    kprintf("type:  <%s>\n", _vid.type);
}

ssize_t fat16_get_file_size(const char *filename)
{
    dir_entry_t *de;

    if (!(de = _rootdir_find_entry(filename)))
        return -1;

    return de->size; 
}

// returns number of bytes read
ssize_t fat16_load(const char *filename, void *buf, const size_t buf_size)
{
    ata_sector_t *sec;
    clu_addr_t clu_num;
    uint32_t file_size;
    uint8_t *pbuf, *psec;
    size_t i, max;

    {
        dir_entry_t *de;

        if (!(de = _rootdir_find_entry(filename)))
            return 1;

        // directory entry containing the file found
        clu_num = de->starting_cluster + ROOT_CLUSTERS;
        file_size = de->size;

        // take the lowest value
        max = file_size < buf_size ? file_size : buf_size;
    }

    for (i = 0, pbuf = (uint8_t *) buf; i < max; i++, pbuf++) {
        if (!(i % sizeof(ata_sector_t))) {
            // update sector
            uint32_t rsec_num = i / sizeof(ata_sector_t) % SECTORS_PER_CLUSTER;

            if (i && !(i % CLUSTER_SIZE)) {
                // update cluster
                clu_num = _fat_get_next_clu_num(clu_num);
                if (!clu_num || 0xffff == clu_num)
                    return -1;
                clu_num += 8;
            }
        
            if (!(sec = _read_cluster_sec(clu_num, rsec_num))) 
                return -1;

            psec = (uint8_t *) sec;
        }
        // copy byte
        *pbuf = *psec++;
    }

    return i; 
}


