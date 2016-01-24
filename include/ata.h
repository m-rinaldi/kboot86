#pragma once

#include <stdint.h>

typedef uint32_t lba_t;

#define ATA_SECTOR_SIZE     512
typedef struct {
    uint8_t data[ATA_SECTOR_SIZE];
} __attribute__((packed)) ata_sector_t;

int ata_init(void);
void ata_display_info(void);
int ata_select_drive(unsigned int drive_num);
int ata_read_sector_chs(unsigned int c, unsigned int h, unsigned int s,
                        ata_sector_t *);
int ata_read_sector_lba(lba_t, ata_sector_t *);

