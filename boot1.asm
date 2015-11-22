;-------------------------------------------------------------------------------
; Second Stage Bootloader
; Jorge Rinaldi <jrg.rinaldi@gmail.com>
;-------------------------------------------------------------------------------

bits 16

org 0x0000
    mov  ax, cs
    mov  ds, ax

    ; set the stack
    mov  ax, 0x9100
    mov  ss, ax
    mov  sp, 0x0fff

    push welcome_str
    call bios_print
    add  sp, 2

hang:
    jmp hang

%include "bios_print.inc"


welcome_str         db 'Second Stage Bootloader', 10, 13, 0

times 4096-($-$$)   db 0
