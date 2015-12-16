OUTPUT_FORMAT("binary")
ENTRY(_start)
phys = 0x7e00;
SECTIONS
{
    .text phys : AT(phys) {
        code = .;
        *(.text)
        *(.rodata)
    }

    .data : AT(phys + (data - code))
    {
        data = .;
        *(.data)
    }
    end = .;
}
