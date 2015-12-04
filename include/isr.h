#ifndef _ISR_H_
#define _ISR_H_

#include <stdint.h>

/* This is the data that is save onto the stack prior entering the handler */
typedef struct {
    /* This is saved by the assembly wrapper function */
    uint16_t    stack_pad_ds;   /* lowest address -> last pushed */
    uint16_t    ds;
    uint32_t    edi;
    uint32_t    esi;
    uint32_t    ebp;
    uint32_t    kernel_esp;
    uint32_t    ebx;
    uint32_t    edx;
    uint32_t    ecx;
    uint32_t    eax;

    uint32_t    isr_nr;
    uint32_t    error_code;

    /* This is saved automatically by the processor */
    uint32_t    eip;
    uint16_t    stack_pad_cs;
    uint16_t    cs;
    uint32_t    eflags;
    uint32_t    user_esp;
    uint16_t    stack_pad_ss;
    uint16_t    ss;     /* highest address -> first pushed */   
} __attribute__((packed)) isr_saved_regs_t;

void isr_register_handler(unsigned int, void (*)(isr_saved_regs_t *));

#endif /* _ISR_H_ */
