#pragma once

#include <ata.h>

int hdd_init(void);
void hdd_display_mbr(void);
int hdd_read_sector(unsigned int pnum, uint32_t rsec_num, ata_sector_t *buf);
