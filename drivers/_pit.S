.text
.globl _pit_isr

.align 4
// PIT ISR wrapper
_pit_isr:
	call pit_isr
    iret
