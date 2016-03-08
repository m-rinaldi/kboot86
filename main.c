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
#include <elf32.h>
#include <jmp.h>
#include <paging.h>

// TODO replace with a more elegant solution
//#define intr(n) asm volatile ("int $" #n : : : "cc", "memory")

#define FILENAME "kernel.elf"
#define BUF_SIZE (1 << 20)
static void *_mem_load_addr = (void *) (2 << 20);

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

    kprintf("initializing paging...");
    if (paging_init())
        goto error;
    kprintf("ok\n");
    
    paging_enable();

    if (hdd_init() || fat16_init(0))
        goto error;

    hdd_display();
    fat16_display_vid();
    fat16_display_root();

    {
        int count;
        uintptr_t jmp_addr;

        kprintf("file size: %d\n", fat16_get_file_size(FILENAME));
        if (-1 == (count = fat16_load(FILENAME, _mem_load_addr, BUF_SIZE))) {
            kprintf("error while loading file %s\n", FILENAME);
            goto error;
        } else {
            kprintf("read: %d bytes\n", count);
        }

        if (!(jmp_addr = elf32_map(_mem_load_addr))) {
            kprintf("error while mapping the ELF image\n");
            goto error;
        }
        kprintf("jmp addr: %x\n", jmp_addr);
        kprintf("vaddr: %x => paddr: %x\n",
                jmp_addr, paging_vaddr2paddr(jmp_addr));

        /* TODO when the IDT is moved, so that interrupts can be triggered
        if (paging_unmap(0))
            goto error;
        */
        jmp(jmp_addr);
    }

    shell_do();

error:
    kprintf("\n*** ERROR! ***");
    while (1)
        ;
}
