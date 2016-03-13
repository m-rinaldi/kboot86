CC = ~/opt/cross/bin/i686-elf-gcc
LD = ~/opt/cross/bin/i686-elf-ld
AS = ~/opt/cross/bin/i686-elf-as
CFLAGS = -c -ffreestanding -Wall -Wextra
INCLUDES = include/

export CC
export AS
export CFLAGS
export LD

.PHONY: all clean

all: kernel.elf

kernel.elf : startup.o kmain.o drivers/drivers.o lib/lib.o shell.o
	$(LD) kmain.o shell.o drivers/drivers.o lib/lib.o -T kernel.ld
	./update_hdd.sh

%.o : %.c 
	$(CC) -c $< $(CFLAGS) -I $(INCLUDES) -o $@

%.o : %.S
	@$(AS) -c $< -o $@

drivers/drivers.o :
	make -C drivers/

lib/lib.o :
	make -C lib/

clean:
	@echo -n cleaning...
	@make -C drivers/ clean
	@make -C lib/ clean
	@rm -f *.o 
	@echo done
