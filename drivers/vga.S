.code32

.set    BASE_ADDR,          0xb8000

.set    NUM_COLS,           80          # putc_ assumes it is equal to 80
.set    NUM_ROWS,           25
.set    NUM_CHARS,          (NUM_COLS*NUM_ROWS)

.set    SCND_ROW_ADDR,      (BASE_ADDR+2*NUM_COLS)
.set    LAST_ROW_ADDR,      (BASE_ADDR+2*(NUM_ROWS-1)*NUM_COLS)
.set    NUM_CHARS2SCROLL,   (NUM_CHARS-NUM_COLS)

.set    ATTR_DEFAULT,       0x0a
.set    SPACE_ASCII,        0x20

.section .data
curs_x:
    .byte   0
curs_y:
    .byte   0
attr:
    .byte   0x0a

.section .text
.globl _vga_init
_vga_init:
    pushw   %dx

    xorb    %dl, %dl
    movb    %dl, curs_x
    movb    %dl, curs_y

    popw    %dx
    ret

.globl _vga_clear
_vga_clear:
    pushl   %eax
    pushl   %ecx
    pushl   %edx

    xorl    %eax, %eax
    xorl    %edx, %edx

    movb    $SPACE_ASCII, %al
    movb    attr, %ah
    movw    %ax, %dx
    shll    $16, %eax
    addl    %eax, %edx

    movl    $BASE_ADDR, %eax
    movl    $NUM_CHARS, %ecx
    shrl    $1, %ecx            # 2 chars for each iteration
_vga_clear_repeat:
    movl    %edx, (%eax)
    addl    $4, %eax
    loop    _vga_clear_repeat
    
    popl    %edx
    popl    %ecx
    popl    %eax
    ret

.globl _vga_print
_vga_print:
    pushl   %ebx
    pushl   %edx

    movl    12(%esp), %ebx

_vga_print_repeat:
    movb    (%ebx), %dl
    testb   %dl, %dl
    jz      _vga_print_end
    call    _putc
    incl    %ebx
    jmp     _vga_print_repeat

    call    _putc
_vga_print_end:
    popl    %edx
    popl    %ebx
    ret

# char is in DL
_putc:
    pushl   %eax
    pushl   %ebx

    xorl    %eax, %eax
    movb    curs_y, %al
    # 80 = 64 + 16
    shll    $6, %eax
    movl    %eax, %ebx
    xorl    %eax, %eax
    movb    curs_y, %al
    shll    $4, %eax
    addl    %eax, %ebx

    xorl    %eax, %eax
    movb    curs_x, %al
    addl    %eax, %ebx

    # EBX contains now the index
    shll    $1, %ebx                # each char takes 2 bytes 
    addl    $BASE_ADDR, %ebx


    movb    %dl, %al
    movb    attr, %ah

    # write the char with the attr code
    movw    %ax, (%ebx)

    incb    curs_x
    movb    $NUM_COLS, %al
    cmpb    curs_x, %al
    jnz     _putc_end
    call    _line_feed
_putc_end:
    popl    %ebx
    popl    %eax
    ret

_line_feed:
    pushl   %eax

    movb    $0, curs_x
    incb    curs_y
    movb    $NUM_ROWS, %al
    cmpb    curs_y, %al
    jnz     _line_feed_end
    call _scroll_down
_line_feed_end:
    popl    %eax
    ret

_scroll_down:
    pushl   %eax
    pushl   %ecx
    pushl   %esi
    pushl   %edi

    # source
    movl    $SCND_ROW_ADDR, %esi

    # dest
    movl    $BASE_ADDR, %edi

    cld
    movl    $NUM_CHARS2SCROLL, %ecx
    rep     movsw

    # TODO clear last (undermost) row
  
    popl    %edi
    popl    %esi
    popl    %ecx
    popl    %eax
    ret
