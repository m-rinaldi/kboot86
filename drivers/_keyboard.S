.text
.globl _keyboard_isr
.align 4
_keyboard_isr:
    pushal
    call keyboard_isr
    popal
    iret
    
