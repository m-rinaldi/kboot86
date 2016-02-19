#include <stdint.h>
#include <stddef.h>

static volatile uint16_t *vga_mem;

#define ATTR    0x1f    // white on blue
void kmain(void)
{
    const char *str = "sample kernel";
    size_t i;

    vga_mem = (uint16_t *) 0xb8000;

    for (i = 0; str[i]; i++)
        vga_mem[i] = (uint16_t) ATTR << 8 | str[i];
    
    while (1)
        ;
}
