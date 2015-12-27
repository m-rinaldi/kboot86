#include <console.h>
#include <pic.h>
#include <idt.h>
#include <intr.h>
#include <pit.h>
#include <keyboard.h>
#include <shell.h>

//XXX
#include <vga.h>

// TODO replace with a more elegant solution
//#define intr(n) asm volatile ("int $" #n : : : "cc", "memory")

void main(void)
{
    console_init();
    console_puts("kboot86");
    pic_remap(32, 40);
    idt_init(0);

    if (pit_init())
        goto error;

    if (keyboard_init())
        goto error;

    pic_enable();
    intr_enable();

    shell_do();
error:
    console_puts("*** ERROR! ***");
}
