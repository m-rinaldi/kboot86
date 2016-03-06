.text
.globl jmp
jmp:
    movl 4(%esp), %eax
    jmp  *%eax 
