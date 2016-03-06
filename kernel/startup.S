.text
.globl _startup
_startup:
    // set up the stack
    movl    $0x00200000, %esp
    jmp kmain  // kernel's entry point
