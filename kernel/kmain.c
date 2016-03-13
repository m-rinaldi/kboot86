#include <stdint.h>
#include <stddef.h>

#include <console.h>
#include <kstdio.h>
#include <intr.h>

#define VGA_MEM_ADDR    0xb8000
#define COLS            80
#define ROWS            25
#define ATTR            0x1f    // white on blue


static inline unsigned int _xy2idx(unsigned int x, unsigned int y)
{
    return y + COLS*x;
}

void bsod(void)
{
    volatile uint16_t *vga_mem;
    int i, j;
    
    vga_mem = (uint16_t *) VGA_MEM_ADDR;
    for (i = 0; i < ROWS; i++)
        for (j = 0; j < COLS; j++)
            vga_mem[_xy2idx(i, j)] =
                (uint16_t) ATTR << 8 | (0x00ff & vga_mem[_xy2idx(i, j)]);

    while (1)
        ;
}

void kmain(void)
{
    //bsod();

    intr_disable();

    console_init();
    kprintf("Hello World!\n");
    while (1)
        ;

#if 0
    pic_init();

    // TODO change interface and storing of the IDT
    idt_init(0x1000);
    
    console_init();
    if (keyboard_init())
        goto error;
#endif

    
}
