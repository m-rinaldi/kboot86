OUTPUT_FORMAT("elf32-i386")
ENTRY(_startup)
STARTUP(startup.o)
OUTPUT(kernel.elf)

SECTIONS
{
    .text.start (0x00100000) : {
        startup.o(.text)
    }

    .text : ALIGN(0x1000) {
        _TEXT_START_ = .;
            *(.text)
        _TEXT_END_   = .;
    }

    .data : ALIGN(0x1000) {
        _DATA_START_ = .;
            *(.data)
        _DATA_END_   = .;
    }

    .bss : ALIGN(0x1000) {
        _BSS_START_ = .;
            *(.bss)
        _BSS_END_   = .;
    }
}

