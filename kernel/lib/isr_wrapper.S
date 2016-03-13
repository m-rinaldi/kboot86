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

// XXX
.align 4
.globl page_fault
page_fault:
    pushl   $page_fault_msg
    call    console_puts
    addl    $4, %esp
page_fault_loop:
    jmp     page_fault_loop
page_fault_msg:
    .asciz "from page fault exception handler!"
