# kboot86

This project is aimed to implement a working bootloader for the x86 instruction set architecture.

## Boot stages
The following boot stages are targeted:
0. Floppy drive boot sector.
1. Floppy drive: second stage bootloader by means of BIOS interrupt calls.
2. Final bootloader in *protected mode*. Actual HDD driver, no BIOS interrupt calls.
3. Load OS kernel from a FAT16/FAT32 partition via ATA PIO.
