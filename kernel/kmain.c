#include <stdint.h>
#include <stddef.h>

#include <console.h>
#include <kstdio.h>
#include <intr.h>
#include <pic.h>
#include <idt.h>
#include <keyboard.h>
#include <shell.h>

#define VGA_MEM_ADDR    0xb8000
#define COLS            80
#define ROWS            25
#define ATTR            0x1f    // white on blue

// XXX
//static uint8_t bloater[1 << 20];

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
    
    idt_init();
    pic_init();

    console_init();
    if (keyboard_init())
        goto error;

    kprintf("kernel86\n");

    intr_enable();

    shell_do();

error:
    kprintf("error");
    while (1)
        ;
    
}
