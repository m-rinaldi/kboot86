#ifndef _ISR_H_
#define _ISR_H_

#include <stdint.h>

// lowest positon -> last pushed

typedef struct {
    uint32_t    eip;
    uint16_t    pad0;       // 32-bit aligned
    uint16_t    cs;
    uint32_t    eflags;
} __attribute__((packed)) intr_stack_t;

void isr_register_handler(unsigned int, void (*)(intr_stack_t *));

#endif /* _ISR_H_ */
