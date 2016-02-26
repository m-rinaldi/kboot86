.text
.globl paging_enable
paging_enable:
    movl %cr0, %eax
    orl  $0x80000000, %eax
    movl %eax, %cr0
    ret
