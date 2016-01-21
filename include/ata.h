#pragma once

typedef unsigned long int lba_t;

int ata_init(void);
int ata_select_drive(unsigned int drive_num);
int ata_read_chs_sector(unsigned int, unsigned int, unsigned int, void *buf);

