#ifndef _INTR_H_
#define _INTR_H_

#include <stdint.h>

static inline
void intr_enable(void)
{
	asm volatile
        (
            "# set IF in EFLAGS\n\t"
            "sti\n\t"
                    :   // no output
                    :   // no input
                    :   "cc", "memory"
        );
}

static inline
void intr_disable(void)
{
	asm volatile 
        (
            "# clear IF in EFLAGS\n\t"
            "cli\n\t"
                    :   // no output
                    :   // no input
                    :   "cc", "memory"
        );
}

// generate a software interrupt
static inline
void intr(const uint8_t intr_num)
{
    // contains self-modifying code
    asm volatile
        (
            "# generate a software interrupt\n\t"
            "# the interrupt number in the INT opcode is modified\n\t"
            "movb   %0, 1+int_pos\n"
            "int_pos:\n\t"
            "int $0\n\t"
                : 
                : "r" (intr_num)
                : "cc", "memory"
        );
}

int intr_register_irq(uint8_t irq_num, void (*isr)(void));


#endif // _INTR_H_
