# XXX
OBJS := drivers/drivers.o lib/lib.o mm/mm.o test/test.o main.o bsh/bsh.o

DIRS := lib mm drivers bsh

# XXX
SUBDIRS := drivers lib mm test

# TODO move to a .mk only included by this file
# image to be built is a 1.44MB floppy
# C x H x S x sector_size = 80 x 2 x 18 x 512B = 1440kB
NUM_CYLINDERS       := 80
NUM_HEADS           := 2
NUM_TRACKS          := $(shell expr $(NUM_CYLINDERS) '*' $(NUM_HEADS))
SECTORS_PER_TRACK   := 18
SECTOR_SIZE         := 512
TRACK_SIZE          := $(shell expr $(SECTORS_PER_TRACK) '*' $(SECTOR_SIZE))
FLOPPY_SIZE         := $(shell expr $(NUM_TRACKS) '*' $(TRACK_SIZE))

# XXX
KBOOT86_SIZE_MAX := $(shell expr $(SECTOR_SIZE) '*' 18 '*' 4)


export CC
export AS
export CFLAGS
export LD

TARGET := floppy.img

# TODO change to boot3.bin and do away with the ".bin" suffixes

.PHONY: all clean check_image_size

all: $(TARGET) check_image_size

include common.mk

check_image_size: IMAGE_SIZE = $(shell cat ${TARGET}.size)
check_image_size: $(TARGET).size
	@echo -n checking floppy image size...
	@echo floppy <'$(TARGET)'> image size: '$(IMAGE_SIZE)'
	@test '$(IMAGE_SIZE)' -eq '$(FLOPPY_SIZE)' || (echo; \
        echo size '$(IMAGE_SIZE)'B, expected '$(FLOPPY_SIZE)'B; exit 1)
	@echo 'OK'

# TODO add the target kboot86 as the concatenation of boot0, boot1 and boot2
$(TARGET): kboot86.bin floppy.pad
	@echo -n generating '$@'...
	@cat $^ > '$@'
	@echo done

kboot86.bin: boot0.bin boot1.bin track0.pad boot2.bin
	@echo -n generating '$@'...
	@cat $^ > '$@'
	@echo done
	
%.size: %
	@echo -n generating '$@...'
	@wc -c '$<' | cut -f1 -d' ' > '$@'
	@echo done

floppy.pad: SIZE            = $(shell cat '$<')
floppy.pad: PADDING_SIZE    = $(shell expr $(FLOPPY_SIZE) - ${SIZE})
floppy.pad: kboot86.bin.size
	@echo -n generating padding for the floppy image...
	@test $(SIZE) -le $(FLOPPY_SIZE) || exit 1
	@dd if=/dev/zero of=$@ count=1 bs=$(PADDING_SIZE)
	@echo done

track0.pad: SIZE            = $(shell expr $$(cat $<) '+' $$(cat ${word 2, $^}))
track0.pad: PADDING_SIZE    = $(shell expr $(TRACK_SIZE) '-' ${SIZE})
track0.pad: boot0.bin.size boot1.bin.size
	@echo -n generating padding for the first track...
	@dd if=/dev/zero of=$@ count=1 bs=$(PADDING_SIZE)
	@echo done

hdd.img: hdd_sector.bin hdd.pad
	@cat hdd_sector.bin hdd.pad > $@

hdd.pad: 
	@dd if=/dev/zero of=./hdd.pad bs=512 \
        count=$$(expr 1024 '*' 4 '*' 16 - 1)

hdd_sector.bin: hdd_sector.asm
	@nasm $< -f bin -o $@

# TODO check size limit (image is being loaded at the lowest 1MB)
# TODO include all the object files
boot2.bin: boot2.ld boot2.o jmp.o $(OBJS) #-lgcc
	$(LD) -T $< -o $@ $(filter-out $<, $^) -L $(LDPATH) -lgcc

drivers/drivers.o: 
	@make -C drivers/

lib/lib.o:
	@make -C lib/

mm/mm.o:
	@make -C mm/

bsh/bsh.o:
	@make -C bsh/

test/test.o:
	@make -C test/

%.o: %.c
	@echo -n compiling '$<'...
	@$(COMPILE.c) $(OUTPUT_OPTIION) $<
	@echo 'done'

boot_config.inc: CEILING_ADD    := $(shell expr $(TRACK_SIZE) - 1)
boot_config.inc: SIZE           = $(shell expr $$(cat $(word 2, $^)) + \
                                  ${CEILING_ADD})
boot_config.inc: SIZE_IN_TRACKS = $(shell expr $(SIZE) / ${TRACK_SIZE})
boot_config.inc: gen-boot_config.pl boot2.bin.size
	@echo -n generating '$@'...
	@perl $< $(SIZE_IN_TRACKS) > '$@'
	@echo done
	
# TODO desing a single pattern rule for both next rules

boot0.bin: boot0.asm boot.inc bios_print.inc
	@echo -n 'building 1st stage bootloader: $<...'
	@nasm $< -f bin -o $@
	@echo done

# TODO
boot1.bin: boot1.asm boot.inc bios_print.inc boot_config.inc
	@echo -n 'building 2nd stage bootloader: $<...'
	@nasm $< -f bin -o $@
	@echo done

%.o: %.S
	@echo -n 'compiling $<...'
	@$(COMPILE.s) -o $@ $<
	@echo 'done'

clean:
	@echo -n cleaning...
	@make -C mm/ clean
	@make -C drivers/ clean
	@make -C lib/ clean
	@make -C test/ clean
	@make -C bsh/ clean
	@rm -f *.bin *.o *.pad floppy.img *.log *.size
	@echo done
