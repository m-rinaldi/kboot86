# kboot86
Floppy kernel bootloader for x86-PC.

This project is aimed to implement a working bootloader for a PC with the x86 instruction set architecture.

<p align="center">
<img src="https://raw.githubusercontent.com/m-rinaldi/kboot86/master/misc/screenshot.png" alt="on-Bochs-running kboot86's screenshot">
</p>

## Capabilities
So far, kboot86 loads in 32-bit protected mode an ELF kernel image from a primary FAT16 partition on the master hard drive of the primary ATA bus.

At the moment of loading the kernel, the processor is already in 32-bit protected mode, therefore the loading of the kernel is performed by means of actual ATA HDD drivers and not BIOS insterrupt calls. At this point the MMU has been also set up to support paging.

Jorge Rinaldi
