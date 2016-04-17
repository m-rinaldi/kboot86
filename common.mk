CC := ~/opt/cross/bin/i686-elf-gcc
LD := ~/opt/cross/bin/i686-elf-ld
AS := ~/opt/cross/bin/i686-elf-as
CPPFLAGS += -I include/
CFLAGS := -c -std=gnu99 -ffreestanding -Wall -Wextra


%.o: %.c
	@echo -n 'compiling $<...'
	@$(COMPILE.c) $<
	@echo 'done'

