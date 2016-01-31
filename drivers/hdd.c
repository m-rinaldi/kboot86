#include <hdd.h>

#include <ata.h>
#include <mbr.h>

#include <stdint.h>
#include <stdbool.h>

static bool _within_partition(unsigned int pnum, uint32_t sec_num_lba)
{
#if 0
    if (!_partition[pnum].valid)
        return false;

    if (sec_num_lba < _partition[pnum].starting_sec_lba             ||
        sec_num_lba - partition[pnum].starting_sec_lba >= num_secs)
    {
        return false;
    }
#endif
    return true;
}

int hdd_init(void)
{
    if (ata_init())
        return 1;

    if (mbr_init())
        return 1;

    return 0;
}

void hdd_display(void)
{
    ata_display_info();
    mbr_display(); 
}


