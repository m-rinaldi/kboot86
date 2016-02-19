CC = ~/opt/cross/bin/i686-elf-gcc
LD = ~/opt/cross/bin/i686-elf-ld
AS = ~/opt/cross/bin/i686-elf-as
CFLAGS = -c -ffreestanding -Wall -Wextra
INCLUDES = include/

OBJ = drivers/drivers.o lib/lib.o test/test.o main.o shell.o

KBOOT86_SIZE_MAX = $(shell expr 512 '*' 18 '*' 3)

export CC
export AS
export CFLAGS
export LD

.PHONY: all clean

all: floppy.img

floppy.img : boot0.bin boot1.bin floppy.pad track0.pad track3.pad kboot86.bin
	@echo -n generating floppy image...
	@cat boot0.bin boot1.bin track0.pad kboot86.bin track3.pad floppy.pad > floppy.img
	@echo done

floppy.pad track0.pad track3.pad : kboot86.bin
	#@echo -n "generating the padding for 1st track of the floppy..."
	dd if=/dev/zero of=./track0.pad bs=512 count=9 2>/dev/null
	#@echo done
	#@echo -n "generating the padding for 4th track of the floppy..."
	dd if=/dev/zero of=./track3.pad bs=1 count=$$(expr ${KBOOT86_SIZE_MAX} - $$(stat -c %s ./kboot86.bin)) 2>/dev/null
	#@echo done
	#@echo -n "generating the padding for the floppy image..."
	dd if=/dev/zero of=./floppy.pad bs=512 count=2808 2>/dev/null
	#@echo done

hdd.img : hdd_sector.bin hdd.pad
	@cat hdd_sector.bin hdd.pad > $@

hdd.pad : 
	@dd if=/dev/zero of=./hdd.pad bs=512 \
        count=$$(expr 1024 '*' 4 '*' 16 - 1)

hdd_sector.bin : hdd_sector.asm
	@nasm $< -f bin -o $@

# TODO include all the object files
kboot86.bin : _kboot86.o jmp.o $(OBJ)
	@$(LD) -T kboot86.ld -o kboot86.bin _kboot86.o jmp.o $(OBJ)

drivers/drivers.o : 
	@make -C drivers/

lib/lib.o :
	@make -C lib/

test/test.o :
	@make -C test/

%.o : %.c
	$(CC) -c $< $(CFLAGS) -I $(INCLUDES) -o $@

%.o : %.S
	@$(AS) -c $< -o $@ 
    
#_kboot86.o : _kboot86.S
#	@$(AS) -c $< -o $@ 

boot1.bin : boot1.asm
	@echo -n "compiling the 2nd stage bootloader: $<..."
	@nasm $< -f bin -o $@
	@echo done

boot0.bin : boot0.asm
	@echo -n "compiling the 1st stage bootloader: $<..."
	@nasm $< -f bin -o $@
	@echo done

clean:
	@echo -n cleaning...
	@make -C drivers/ clean
	@make -C lib/ clean
	@make -C test/ clean
	@rm -f *.bin *.o *.pad floppy.img *.log
	@echo done
