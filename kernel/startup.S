.text
.globl startup
startup:

    // XXX
    movb $'B', %al
    movb $0x1f, %ah
    movl $0xb8000, %edx
    addl $2, %edx
    movw %ax, (%edx)

    // set up the stack
    movl    $0x00200000, %esp
    jmp kmain  // kernel's entry point
