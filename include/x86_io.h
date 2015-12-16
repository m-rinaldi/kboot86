#ifndef _X86_IO_H_
#define _X86_IO_H_

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
uint8_t x86_inb(uint16_t port)
{
    uint8_t value;
    // inb $imm8, %al
    // inb %dx, %al
    asm volatile ("inb %1, %0" : "=a" (value) : "Nd" (port));
    return value;
}

static inline
uint16_t x86_inw(uint16_t port)
{
    uint16_t value;
    // inw $imm8, %ax
    // inw %dx, %ax
    asm volatile ("inw %1, %0" : "=a" (value) : "Nd" (port));
    return value;
}

static inline
void x86_outb(uint16_t port, uint8_t value)
{
    // outb %al, $imm8
    // outb %al, %dx
    asm volatile ("outb %1, %0" : : "Nd" (port),  "a" (value));
    return;
}

static inline
void x86_outw(uint16_t port, uint16_t value)
{
    // outw %ax, $imm8
    // outw %ax, %dx
    asm volatile ("outw %1, %0" : : "Nd" (port), "a" (value));
    return;
}

#endif // _X86_IO_H_
