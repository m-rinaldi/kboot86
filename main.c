#include <console.h>
#include <pic.h>
#include <idt.h>
#include <intr.h>
#include <pit.h>
#include <keyboard.h>
#include <shell.h>
#include <kstdio.h>
#include <hdd.h>

// TODO replace with a more elegant solution
//#define intr(n) asm volatile ("int $" #n : : : "cc", "memory")

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

    if (hdd_init())
        goto error;

    hdd_display();

    shell_do();

error:
    kprintf("*** ERROR! ***");
    while (1)
        ;
}
