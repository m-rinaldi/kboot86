#include <faults.h>

#include <vga.h>
#include <console.h>

void exception_handler(void)
{
    vga_set_bsod();
    console_puts_err("hardware exception!");

    while (1)
        ; 
}
