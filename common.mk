CC := ~/opt/cross/bin/i686-elf-gcc
LD := ~/opt/cross/bin/i686-elf-ld
CPPFLAGS += -I include/
CFLAGS := -c -std=c11 -ffreestanding -Wall -Wextra


%.o: %.c
	@echo -n 'compiling $<...'
	@$(COMPILE.c) $<
	@echo 'done'

