.globl isr_wrapper
.align 4
isr_wrapper:
/*
    pushad
    cld
*/
    /* TODO call ISR in C */
    //popad
    iret

.globl dummy_isr
.align 4
dummy_isr:
    pushl   $msg
    call    console_puts
    addl    $4, %esp
    iret
msg:
    .asciz "from ISR!"
