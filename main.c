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

static ata_sector_t _buf;

void main(void)
{
    intr_disable();

    pic_init();
    idt_init(0);

    if (pit_init())
        goto error;

    console_init();

    // TODO keyboard belongs to console
    if (keyboard_init())
        goto error;
    
    kprintf("kboot86\n");

    intr_enable();

    int test_hdd(void);
    if (!test_hdd())
        kprintf("OK!\n");
    else
        kprintf("Error!\n");

    ata_display_info();

    shell_do();

error:
    kprintf("*** ERROR! ***");
    while (1)
        ;
}
