#include <kstdio.h>
#include <ata.h>

#include <stdint.h>

#define CYLINDERS_NUM   1024
#define HEADS_NUM       4
#define SECTORS_NUM     16

#define LEN (512/sizeof(uint32_t))
typedef uint32_t sector_t[LEN];

static uint32_t _counter;
static sector_t _sector;

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

    for (i = 0; i < sizeof(_sector) / sizeof(uint32_t); i++)
        if (_sector[i] != _counter++)
            return 1;

    return 0;
}


int test_hdd(void)
{
    unsigned int c, h, s;

    if (_init())
        goto err;

    for (c = 0; c < CYLINDERS_NUM; c++) {
        kprintf("at cylinder %6d\n", c);
        for (h = 0; h < HEADS_NUM; h++)
            for (s = 1; s <= SECTORS_NUM; s++) {
                if (ata_read_chs_sector(c, h, s, _sector))
                    goto err;
                if (_check_sector())
                    goto err;
            }
    }

    return 0;

err:
    return 1;
}


