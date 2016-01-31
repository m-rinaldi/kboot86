#include <mbr.h>

#include <ata.h>

#include <stdbool.h>
#include <stddef.h>
#include <kstdio.h>

typedef struct {
    uint8_t     status;
    uint8_t     first_chs[3];
    uint8_t     type;
    uint8_t     last_chs[3];
    uint32_t    first_lba;
    uint32_t    num_sectors;
} __attribute((packed)) partition_entry_t;

#define NUM_PARTITIONS_MAX  4
typedef struct {
    uint8_t             bootstrap[436];
    uint8_t             unique_disk[10];
    partition_entry_t   partition[NUM_PARTITIONS_MAX];
    uint16_t            signature;
} __attribute__((packed)) mbr_t;

// in-memory copy of a partition entry
typedef struct {
    bool        valid;
    bool        bootable;
    uint32_t    starting_sec_lba;
    uint32_t    num_secs;
} mpe_t;

static mpe_t _partition_entry[NUM_PARTITIONS_MAX];
static bool _bootable_partition_found;

// check whether all the bytes that make up the given partition entry are zero
static bool _is_all_zero(const partition_entry_t *pe)
{
    uint8_t *p;
    size_t i;

    for (i = 0, p = (uint8_t *) pe; i < sizeof(*pe); i++, p++)
        if (*p)
            return false;

    return true;
}

static inline
bool _is_bootable(const partition_entry_t *pe)
{
    if (0x80 == pe->status)
        return true;

    return false;
}

static
int _set_partition_info(unsigned int pnum, const partition_entry_t *pe)
{
    // if an entry of the partition table is unused, 
    // then it should be set to all 0
    if (_is_all_zero(pe)) {
        _partition_entry[pnum].valid = false;
        return 0;
    }

    _partition_entry[pnum].starting_sec_lba = pe->first_lba;
    _partition_entry[pnum].num_secs = pe->num_sectors;

    // set whether partition is bootable
    if (_is_bootable(pe)) {
        // any one of the partitions may be bootable,
        // but at most one partition should be bootable
        if (_bootable_partition_found)
            return 1;
        _bootable_partition_found = true;
        _partition_entry[pnum].bootable = true;
    } else
        _partition_entry[pnum].bootable = false;
    
    _partition_entry[pnum].valid = true;
    return 0;
}

int mbr_init(void)
{
    // TODO move this out from the stack
    mbr_t mbr;
    partition_entry_t *pe;
    size_t i;

    for (i = 0; i < NUM_PARTITIONS_MAX; i++)
        _partition_entry[i].valid = false;
    
    if (sizeof(mbr_t) != sizeof(ata_sector_t))
        return 1;
    if (ata_read_sector_lba(0, (ata_sector_t *) &mbr))
        return 1;

    _bootable_partition_found = false;
    for (i = 0, pe = mbr.partition; i < NUM_PARTITIONS_MAX; i++, pe++)
        _set_partition_info(i, pe);

    return 0;
}

int mbr_get_partition_info(unsigned int pnum,
                           uint32_t *start_sec, uint32_t *num_secs)
{
    if (!_partition_entry[pnum].valid)
        return 1;   // unused entry

    if (start_sec)
        *start_sec = _partition_entry[pnum].starting_sec_lba;
    
    if (num_secs)
        *num_secs = _partition_entry[pnum].num_secs;

    return 0;
}

static void _display_partition_entry(const mpe_t *pe)
{
    if (!pe->valid) {
        kprintf("unused\n");
        return;
    }

    if (pe->bootable)
        kprintf(" * \n");
    kprintf("starting sector lba:   %8d\n", pe->starting_sec_lba);
    kprintf("size in sectors:       %8d\n", pe->num_secs);
}

void mbr_display(void)
{
    size_t i;

    for (i = 0; i < NUM_PARTITIONS_MAX; i++) {
        kprintf("partition %1d\n", i);
        _display_partition_entry(&_partition_entry[i]);
    }
}

