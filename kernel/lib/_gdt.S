.text
.globl _gdtr_load

.align 4
_gdtr_load:
	push %eax
	lgdt _gdtr
	movw $0x0010, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	movl %esp, %eax
	movl %eax, %esp
	ljmp $0x0008,$far_jump	/* Just to reload CS */
far_jump:
	pop %eax
	ret

.align 4
_gdtr:
	.word 24
	.long 0x100000
