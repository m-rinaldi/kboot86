;-------------------------------------------------------------------------------
; First Stage Bootloader
; Jorge Rinaldi <jrg.rinaldi@gmail.com>
;-------------------------------------------------------------------------------

bits 16

%include "bootloader.inc"

FIRST_SECTOR2LOAD   equ     2   ; sectors start counting from 1, not 0
NUM_SECTORS2LOAD    equ     8
SECTOR_NUM_MAX      equ     (NUM_SECTORS2LOAD+FIRST_SECTOR2LOAD-1)
SECTOR_SIZE         equ     512

BUF_SEG             equ     (BL_STAGE1_ADDR/16)

; this code will be loaded by the BIOS at address 0x7c00 
org 0x7c00
    ; some BIOS's set CS:IP to 0x07c0:0x0000
    ; some other BIOS's set them to 0x0000:0x7c00
    ; both of them map to the same physical address

    ; in x86 there are three types of JMP instructions:
    ;   1) short (relative to current IP)
    ;   2) near (within the current segement determined by CS, only IP is given)
    ;   3) far  (it additionally includes the CS)

    ; for this code to work with near jumps:
    ;   a) CS:IP = 0x0000:0x7c00 and ORG 0x7c00
    ;   b) CS:IP = 0x07c0:0x0000 and no ORG present

    ; we have chosen the first approach
    ; we set the CS:IP by means of a long jump
    jmp  0x0000:_
_:

    xor  ax, ax
    mov  ds, ax

    ; set up the stack
    mov  ss, ax
    mov  sp, stack_top
   
stack_top:
    push welcome_str
    call bios_print
    add  sp, 2

    ; intialize floppy/HDD
    push init_drive_str
    call bios_print
    add sp, 2

    mov  al, 0
    mov  dl, DRIVE_NUM
    int 0x13
    jc  error
    push ok_str
    call bios_print
    add sp, 2

    ; read sectors from floppy to main memory
repeat_sector_read:
    push read_sector_str
    call bios_print
    add  sp, 2
    call read_sector

    push ok_str
    call bios_print
    add  sp, 2

    mov  al, [sector_num]
    cmp  al, SECTOR_NUM_MAX
    jle  repeat_sector_read 

    ; jump to the just loaded code
    jmp  BUF_SEG:0x0000

    jmp  hang

read_sector:
    mov  ah, 0x02               ; BIOS service
    mov  al, 1                  ; number of sectors to read
    mov  ch, 0                  ; cylinder number
    mov  cl, [sector_num]       ; sector number
    mov  dh, 0                  ; head number
    mov  dl, DRIVE_NUM          ; drive number
    mov  bx, BUF_SEG            ; buffer segment
    mov  es, bx
    mov  bx, [buf_off]
    int  0x13
    jc   error

    ; update sector number
    inc  cl
    mov  [sector_num], cl
    mov  [sector_num_str], cl
    mov  al, '0'
    add  [sector_num_str], al 

    ; update the buffer offset
    add  bx, SECTOR_SIZE
    mov  [buf_off], bx

    ret

%include "bios_print.inc"


error:
    push error_str
    call bios_print
    add sp, 2
hang:
    jmp hang

welcome_str         db  '1st Stage Bootloader', 10, 13, 0
init_drive_str      db  '  Initializating drive...', 0
ok_str              db  'OK', 10, 13, 0
error_str           db  'error', 10, 13, 0
read_sector_str     db  '  loading sector no. ' 
sector_num_str      db  (FIRST_SECTOR2LOAD+'0'), '...', 0
sector_num          db  FIRST_SECTOR2LOAD
buf_off             dw  0

; fill up the rest with zeroes
times 510-($-$$)    db 0

; bootable disk signature
db 0x55
db 0xaa
