#ifndef _ISR_H_
#define _ISR_H_

#include <stdint.h>

// TODO
/*
    if the interrupt is in a higher privilege level, SS and ESP are first 
    pushed to the stack, each one consuming 4 bytes.
    For the same level this step is skipped.
 */

// lowest positon -> last pushed
typedef struct {
    uint32_t    eip;

    uint16_t    pad;    // 32-bit aligned
    uint16_t    cs;

    uint32_t    eflags;
} __attribute__((packed)) intr_stack_t;

void isr_register_handler(unsigned int, void (*)(intr_stack_t *));

#endif /* _ISR_H_ */
