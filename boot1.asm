;-------------------------------------------------------------------------------
; Second Stage Bootloader
; Jorge Rinaldi <jrg.rinaldi@gmail.com>
;-------------------------------------------------------------------------------

bits 16

%include "bootloader.inc"

; for a 1.44MB floppy
; C x H x S x sector_size = 80 x 2 x 18 x 512B = 1440kiB
NUM_SECTORS_PER_TRACK       equ     18
NUM_TRACKS                  equ     80

org 0x0000

    ; CS=BL_STAGE1 even if a third-party first stage bootloader was used
    jmp  (BL_STAGE1_ADDR/16):start
start:
    ; set the data segment
    mov  ax, cs
    mov  ds, ax

    ; set the stack
    mov  ss, ax
    mov  sp, stack_top
stack_top:

    push welcome_str
    call bios_print
    add  sp, 2

    push load_track_str
    call bios_print
    add  sp, 2

    call load_track

    push ok_str
    call bios_print
    add  sp, 2

    ; disable interrupts
    cli

    ; copy the loaded code to 0 address
    ; ds:si src addr
    ; es:di dst addr


    ; set segments
    xor  ax, ax
    mov  es, ax
    mov  ax, 0x1000
    mov  ds, ax
    ; set offsets
    xor  si, si
    xor  di, di
    mov  cx, 9216/2     ; 9kiB to copy
    cld                 ; direction: increasing addresses
    rep  movsw
    
    ; restore DS in order to use the labels here
    mov  ax, cs
    mov  ds, ax

    ; load IDT and GDT
    lidt [idtr_48]
    lgdt [gdtr_48]

    ; TODO check first whether A20 is already enabled

    
    ; enable the A20 line
    in   al, 0x92
    or   al, 2
    out  0x92, al

    ; TODO re-program the 8259 PIC

    ; disable the PIC
    mov  al, 0xff
    out  0xa1, al
    out  0x21, al

    ; enable PE bit
    mov  ax, 0x0001
    lmsw ax

    ; far jump to switch to PM
    jmp  0x0008:0               ; index = 1 -> 2nd entry in the GDT

hang:
    jmp hang

%include "bios_print.inc"

;-------------------------------------------------------------------------------
; load_track
; reads the whole track represented by [cylinder_num] and [head_num] in
; [buf_seg]:[buf_off] and updates [cylinder_num], [head_num] and [buf_off]
;-------------------------------------------------------------------------------
cylinder_num    db  0
head_num        db  1
buf_seg         dw  0x1000
buf_off         dw  0
load_track:
    mov  ax, [buf_seg]
    mov  es, ax
    mov  bx, [buf_off]
    mov  al, NUM_SECTORS_PER_TRACK      ; number of sectors to read
    mov  ch, [cylinder_num]
    mov  cl, 0x01                       ; first sector to read
    mov  dh, [head_num]
    mov  dl, DRIVE_NUM
    mov  ah, 0x02                       ; BIOS function number
    int  0x13
    jc   error
    ; check number of sectors that were read
    cmp  al, NUM_SECTORS_PER_TRACK
    jne  error

    ret

error:
    push error_str
    call bios_print
    add  sp, 2
    jmp  hang

welcome_str     db '2nd Stage Bootloader', 10, 13, 0
load_track_str  db 'loading track...', 0            
error_str       db 'error', 0
ok_str          db 'OK', 10, 13, 0

; TODO alignment?
idtr_48:
    dw 0
    dw 0
    dw 0
gdtr_48:
    dw 0x0024       ; limit (24 bytes -> 3 descriptors)
    dw gdt          ; base address 0x90000 + gdt
    dw 0x0009       ; TODO make this position-independent

; TODO alignment?
gdt:
    ; the null descriptor
    dw 0, 0, 0, 0
    ;--------------------
    dw 0xffff
    dw 0x0000
    dw 0x9a00
    dw 0x00cf
    ;-------------------
    dw 0xffff
    dw 0x0000
    dw 0x9200
    dw 0x00cf

times 4096-($-$$)   db 0
