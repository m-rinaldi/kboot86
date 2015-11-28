AS = nasm

.PHONY: all clean

all: floppy.img

floppy.img : boot0.bin boot1.bin floppy.pad track0.pad  kboot86.bin
	@echo -n generating floppy image...
	@cat boot0.bin boot1.bin track0.pad kboot86.bin floppy.pad > floppy.img
	@echo done

# kboot86.bin <--> track1.pad
floppy.pad track0.pad :
	@echo -n "generating the padding for 1st track of the floppy..."
	@dd if=/dev/zero of=./track0.pad bs=512 count=9 2>/dev/null
	@echo done
	@echo -n "generating the padding for the floppy image..."
	@dd if=/dev/zero of=./floppy.pad bs=512 count=2844 2>/dev/null
	@echo done

hdd.img : hdd_sector.bin hdd.pad
	cat hdd_sector.bin hdd.pad > $@

hdd.pad : 
	dd if=/dev/zero of=./hdd.pad bs=512 \
        count=$$(expr 1024 '*' 4 '*' 16 - 1)

hdd_sector.bin : hdd_sector.asm
	@$(AS) $< -f bin -o $@

kboot86.bin : kboot86.S drivers/vga.S
	@echo -n "compiling the bootloader: $<..."
	~/opt/cross/bin/i686-elf-as kboot86.S drivers/vga.S -o kboot86.o --warn
	~/opt/cross/bin/i686-elf-ld -T kboot86.ld -o kboot86.bin kboot86.o
	@echo done

boot1.bin : boot1.asm
	@echo -n "compiling the 2nd stage bootloader: $<..."
	@$(AS) $< -f bin -o $@
	@echo done

boot0.bin : boot0.asm
	@echo -n "compiling the 1st stage bootloader: $<..."
	@$(AS) $< -f bin -o $@
	@echo done

clean:
	@echo -n cleaning...
	@rm -f *.bin *.o *.pad *.img *.log
	@echo done
