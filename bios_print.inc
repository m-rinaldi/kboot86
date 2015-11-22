;-------------------------------------------------------------------------------
; BIOS print
; Jorge Rinaldi <jrg.rinaldi@gmail.com>
;
; prints a zero-terimated string by calling BIOS services
;-------------------------------------------------------------------------------

bits 16

bios_print:
    ; save registers to be clobbered
    push ax 
    push bp
    push si
    
    mov  bp, sp
    add  bp, 8
    mov  si, [bp]
bios_print_loop:
    mov  al, [si]
    or   al, al     ; zero => end of string
    jz   bios_print_end
    mov  ah, 0x0e
    int  0x10
    inc  si         ; next character
    jmp  bios_print_loop
bios_print_end:
    ; retrieve saved registers
    pop  si
    pop  bp
    pop  ax
    ret

