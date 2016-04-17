CC = ~/opt/cross/bin/i686-elf-gcc
LD = ~/opt/cross/bin/i686-elf-ld
AS = ~/opt/cross/bin/i686-elf-as
CFLAGS = -c -ffreestanding -Wall -Wextra
INCLUDES = include/

OBJ = drivers/drivers.o lib/lib.o mm/mm.o test/test.o main.o bsh/bsh.o

# TODO just include the number of the subdirectories and the rest should be 
#      done automatically
SUBDIRS = drivers lib mm test

KBOOT86_SIZE_MAX = $(shell expr 512 '*' 18 '*' 4)

export CC
export AS
export CFLAGS
export LD

.PHONY: all clean

all: floppy.img

# TODO check that the floppy image is exactly 1440 kiB in size
# otherwise show up an error


# TODO track4.pad: the track number should be generated automatically
floppy.img : boot0.bin boot1.bin floppy.pad track0.pad track4.pad kboot86.bin
	@echo -n generating floppy image...
	@cat boot0.bin boot1.bin track0.pad kboot86.bin track4.pad floppy.pad > floppy.img
	@echo done

# TODO track0.pad will be always generated the same way, create rule just for it
# TODO track4.pad
floppy.pad track0.pad track4.pad : kboot86.bin
	#@echo -n "generating the padding for 1st track of the floppy..."
	dd if=/dev/zero of=./track0.pad bs=512 count=9 2>/dev/null
	#@echo done
	#@echo -n "generating the padding for 5th track of the floppy..."
	dd if=/dev/zero of=./track4.pad bs=1 count=$$(expr ${KBOOT86_SIZE_MAX} - $$(stat -c %s ./kboot86.bin)) 2>/dev/null
	#@echo done
	#@echo -n "generating the padding for the floppy image..."
	# TODO re-calculate by hand
	dd if=/dev/zero of=./floppy.pad bs=512 count=2790 2>/dev/null
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

mm/mm.o :
	@make -C mm/

bsh/bsh.o:
	@make -C bsh/

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
	@make -C mm/ clean
	@make -C drivers/ clean
	@make -C lib/ clean
	@make -C test/ clean
	@make -C bsh/ clean
	@rm -f *.bin *.o *.pad floppy.img *.log
	@echo done
