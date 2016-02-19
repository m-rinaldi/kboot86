.text
.globl jmp
jmp:

    // XXX
    movb $'A', %al
    movb $0x1f, %ah
    movl $0xb8000, %edx
    movw %ax, (%edx)
  
    movl 4(%esp), %eax
    jmp  *%eax 
