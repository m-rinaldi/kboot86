.text
.globl _pit_isr

.align 4
// PIT ISR wrapper
_pit_isr:
    pushal
	call pit_isr
    popal
    iret
