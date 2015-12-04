#ifndef _ATA_H_
#define _ATA_H_

typedef unsigned long int lba_t;


int ata_select_drive(unsigned int drive_num);
int ata_read_chs_sector(unsigned int c, unsigned int h, unsigned int s,
                        void *buf);

#endif //_ATA_H_
