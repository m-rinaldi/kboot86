#include <console.h>
#include <pic.h>
#include <idt.h>
#include <intr.h>
#include <pit.h>
#include <keyboard.h>
#include <shell.h>
//#include <kstdio.h>

// TODO replace with a more elegant solution
//#define intr(n) asm volatile ("int $" #n : : : "cc", "memory")

void main(void)
{
    intr_disable();

    console_init();
    //kprintf("kboot86\n");

    pic_init();
    idt_init(0);

    if (pit_init())
        goto error;

    // TODO keyboard belongs to console
    if (keyboard_init())
        goto error;

    intr_enable();

    shell_do();

error:
    kprintf("*** ERROR! ***");
}
