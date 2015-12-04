.globl isr_wrapper

.align 4
isr_wrapper:
    pushad
    cld
    /* TODO call ISR in C */
    popad
    iret
