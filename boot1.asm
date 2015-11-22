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

hang:
    jmp hang

%include "bios_print.inc"

;-------------------------------------------------------------------------------
; load_track
; reads the whole track represented by [cylinder_num] and [head_num] in
; [buf_seg]:[buf_off] and updates [cylinder_num], [head_num] and [buf_off]
;-------------------------------------------------------------------------------
cylinder_num    db  1
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

    ; update the buffer pointer
    mov  ax, NUM_SECTORS_PER_TRACK
    shl  ax, 9
    mov  bx, [buf_off]
    add  bx, ax
    mov  [buf_off], bx

    ; update the cylinder number
    mov  al, [head_num]
    cmp  al, 1
    jne  preserve_cylinder_num
    inc  byte [cylinder_num]
preserve_cylinder_num:
    ; update the head number (disk side)
    mov  al, 1
    sub  al, [head_num]
    mov  [head_num], al
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

times 4096-($-$$)   db 0
