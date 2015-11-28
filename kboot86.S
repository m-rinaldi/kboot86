.code32

.section .text
.globl _start
.org 0
_start:
    # set selectors
    movw    $0x0010, %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %fs
    movw    %ax, %gs
    movw    %ax, %ss

    # set up the stack
    movl    $0x0009ffff, %esp

    call    _vga_init
    call    _vga_clear

    pushl   $msg
    call    _vga_print
    addl    $4, %esp

hang:
    jmp hang

.section .data

msg:
    .asciz  "kboot86"
