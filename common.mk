CC := ~/opt/cross/bin/i686-elf-gcc
LD := ~/opt/cross/bin/i686-elf-ld

# TODO remove
AS := ~/opt/cross/bin/i686-elf-as

CPPFLAGS    += -I include/
ifdef USERSPACE_HOSTED
  CPPFLAGS += -DUSERSPACE_HOSTED
endif

CFLAGS      := -c -std=gnu11 -ffreestanding -nostdlib -Wall -Wextra
LDPATH      := ../opt/cross/lib/gcc/i686-elf/4.9.3/

%.o: %.c
	@echo -n 'compiling $<...'
	@$(COMPILE.c) $<
	@echo 'done'

