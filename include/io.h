#ifndef _IO_H_
#define _IO_H_

#include <stdint.h>

/*
    inb   $imm8, %al
    inb   %dx, %al
 
    inw   $imm8, %ax
    inw   %dx, %ax

    outb  $imm8, %al
    outb  %dx, %al

    these instructions take either an immendiate 8-bit value as a port number
    or a port specified in the d register.

    therefore, asm inline:
        "N":  unsigned 8-bit integer constant
        "d":  the d register
   
    the "value" has to be always contained in the a register.
*/

static inline
uint8_t inb(const uint16_t port)
{
    uint8_t value;

    // inb $imm8, %al
    // inb %dx, %al
    asm volatile
        (
            "# inb()\n\t"
            "inb %1, %0\n\t"
            :   "=a" (value)
            :   "Nd" (port)
        );
    return value;
}

static inline
uint16_t inw(const uint16_t port)
{
    uint16_t value;

    // inw $imm8, %ax
    // inw %dx, %ax
    asm volatile
        (
            "# inw()\n\t"
            "inw %1, %0\n\t"
            :   "=a" (value)
            :   "Nd" (port)
        );
    return value;
}

static inline
void outb(const uint16_t port, const uint8_t value)
{
    // outb %al, $imm8
    // outb %al, %dx
    asm volatile
        (
            "# outb()\n\t"
            "outb %1, %0\n\t"
            :   // no output operands  
            :   "Nd" (port),  "a" (value)
        );
    return;
}

static inline
void outw(const uint16_t port, const uint16_t value)
{
    // outw %ax, $imm8
    // outw %ax, %dx
    asm volatile
        (
            "# outw()\n\t"
            "outw %1, %0\n\t"
            :   // no output operands
            :   "Nd" (port), "a" (value)
        );
    return;
}

#endif // _IO_H_
