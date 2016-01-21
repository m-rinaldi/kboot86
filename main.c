#include <console.h>
#include <pic.h>
#include <idt.h>
#include <intr.h>
#include <pit.h>
#include <keyboard.h>
#include <shell.h>
#include <kstdio.h>
#include <ata.h>

// TODO replace with a more elegant solution
//#define intr(n) asm volatile ("int $" #n : : : "cc", "memory")

static uint8_t _buf[512];

void main(void)
{
    intr_disable();

    pic_init();
    idt_init(0);

    if (pit_init())
        goto error;

    console_init();
    kprintf("kboot86\n");

    // TODO keyboard belongs to console
    if (keyboard_init())
        goto error;

    intr_enable();

    if (ata_init() || ata_select_drive(0) || ata_read_chs_sector(0,0,1,_buf))
        goto error;
    {
        int i;
        
        for (i = 0; i < 16; i++)
            kprintf("%x\n", _buf[i]);
        
    }

    shell_do();

error:
    kprintf("*** ERROR! ***");
    while (1)
        ;
}
