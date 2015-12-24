# kboot86
kernel bootloader for x86

This project is aimed to implement a working bootloader for the x86 instruction set architecture.

## Boot stages
The following boot stages are targeted:

1. Floppy drive boot sector.
2. Floppy drive: second stage bootloader by means of BIOS interrupt calls.
3. Final bootloader in *protected mode*. Actual HDD driver, no BIOS interrupt calls.
4. Load an ELF32 kernel from a FAT16/FAT32 partition via ATA PIO.


Jorge Rinaldi
