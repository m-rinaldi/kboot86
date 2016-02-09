#include <console.h>
#include <pic.h>
#include <idt.h>
#include <intr.h>
#include <pit.h>
#include <keyboard.h>
#include <shell.h>
#include <kstdio.h>
#include <hdd.h>
#include <fat16.h>

// TODO replace with a more elegant solution
//#define intr(n) asm volatile ("int $" #n : : : "cc", "memory")

#define FILENAME "welcome.txt"
#define BUF_SIZE 128 
static char _buf[BUF_SIZE];

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

    if (hdd_init() || fat16_init(0))
        goto error;

    hdd_display();
    fat16_display_vid();
    fat16_display_root();

    {
        int count;
        kprintf("file size: %d\n", fat16_get_file_size(FILENAME));
        if (-1 == (count = fat16_load(FILENAME, _buf, BUF_SIZE))) {
            kprintf("error while loading file %s\n", FILENAME);
        } else {
            kprintf("read: %d bytes\n", count);
            _buf[count-1] = '\0';   // VIM adds a newline at the end
            kprintf("%s: <%s>\n", FILENAME, _buf); 
        }
    }

    shell_do();

error:
    kprintf("*** ERROR! ***");
    while (1)
        ;
}
