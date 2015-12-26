.text
.globl _keyboard_isr
.align 4
_keyboard_isr:
    call keyboard_isr
    iret
    
