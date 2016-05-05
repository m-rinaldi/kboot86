#include <hdd.h>

#include <ata.h>
#include <mbr.h>
#include <kstdio.h>

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

int hdd_init(void)
{
    if (ata_init())
        return 1;

    if (mbr_init())
        return 1;

    return 0;
}

static inline
bool _within_partition(unsigned int pnum, uint32_t sec_num_lba)
{
    uint32_t num_secs;

    if (mbr_get_partition_info(pnum, NULL, &num_secs))
        return false;   // unused partition

    if (sec_num_lba >= num_secs)    // out of partition bounds
        return false;

    return true;
}


static inline
int _rel2abs_sector(unsigned int pnum, uint32_t rsec_num, uint32_t *asec_num)
{
    uint32_t start_asec;
    
    if (!_within_partition(pnum, rsec_num))
        return 1;

    if (mbr_get_partition_info(pnum, &start_asec, NULL))
        return 1;

    *asec_num = start_asec + rsec_num;
    return 0;
}

int hdd_read_sector(unsigned int pnum, uint32_t rsec_num, ata_sector_t *buf)
{ 
    uint32_t asec_num;

    if (_rel2abs_sector(pnum, rsec_num, &asec_num))
        return 1;       

    return ata_read_sector_lba(asec_num, buf);
}

void hdd_display_mbr(void)
{
    mbr_display();
}

