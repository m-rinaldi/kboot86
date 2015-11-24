bits 32

org 0
    ; set selectors
    mov  ax, 0x0010
    mov  ds, ax
    mov  es, ax
    mov  fs, ax
    mov  gs, ax
    mov  ss, ax

    ; set up the stack
    mov  esp, 0x0009ffff

    call scr_clear
    mov  eax, msg
    call scr_print
    
hang:
    jmp hang   

%include "lib/video.asm"

msg                 db  'Hello from Protected Mode', 0 

times 9216-($-$$)   db 0
