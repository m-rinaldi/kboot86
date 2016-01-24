#include <kstdio.h>
#include <ata.h>

#include <stdint.h>

#define CYLINDERS_NUM   1024
#define HEADS_NUM       4
#define SECTORS_NUM     16

static uint32_t _counter;
static ata_sector_t _sector;

static
int _init(void)
{
    if (ata_init() || ata_select_drive(0))
        return 1;

    return 0; 
}


static int _check_sector(void)
{
    unsigned int i;
    uint32_t *p;

    p = (uint32_t *) &_sector;
    for (i = 0; i < sizeof(_sector) / sizeof(uint32_t); i++)
        if (*p++ != _counter++)
            return 1;

    return 0;
}


int test_hdd_chs(void)
{
    unsigned int c, h, s;

    if (_init())
        goto err;

    for (c = 0; c < CYLINDERS_NUM; c++) {
        kprintf("at cylinder %6d\n", c);
        for (h = 0; h < HEADS_NUM; h++)
            for (s = 1; s <= SECTORS_NUM; s++) {
                if (ata_read_sector_chs(c, h, s, &_sector))
                    goto err;
                if (_check_sector())
                    goto err;
            }
    }

    return 0;

err:
    return 1;
}

int test_hdd_lba(void)
{
    unsigned int lba;

    if (_init())
        goto err;

    for (lba = 0; lba < 65536; lba++) {
        if (!(lba % 100))
            kprintf("at lba %6d\n", lba);

        if (ata_read_sector_lba(lba, &_sector))
            goto err;
        if (_check_sector())
            goto err;
    }

err:
    return 1;
}


