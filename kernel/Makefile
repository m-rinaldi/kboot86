CC = ~/opt/cross/bin/i686-elf-gcc
LD = ~/opt/cross/bin/i686-elf-ld
AS = ~/opt/cross/bin/i686-elf-as
CFLAGS = -c -ffreestanding -O2 -Wall -Wextra

.PHONY: all clean

all: kernel.elf

kernel.elf : startup.o kmain.o
	$(LD) kmain.o -T kernel.ld
	./update_hdd.sh

%.o : %.c %.S
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	@echo -n cleaning...
	@rm -f *.o 
	@echo done
