BITS 32

VGA_MEM_BASE_ADDR		equ	0xB8000
NR_COLS				equ	80
NR_ROWS				equ     25
NR_CHARS			equ	NR_COLS*NR_ROWS
SPACE_ASCII_CODE		equ	0x20

;-------------------------------------------------------------------------------
; Data
;-------------------------------------------------------------------------------

;Screen current cursor, 32-bit values
scr_curs_x		dd		0	;Current row
scr_curs_y		dd		0	;Current column

;Character attribute
scr_char_attrib 	db		0x0a	;Green on balck by default
;-------------------------------------------------------------------------------
; scr_clear
; It clears the screen
;-------------------------------------------------------------------------------
scr_clear:
	push eax
	push ecx
	push edx

	xor eax, eax
	xor edx, edx

	mov al, SPACE_ASCII_CODE
	mov ah, [scr_char_attrib]
	mov dx, ax
	shl eax, 16
	add edx, eax

	mov eax, VGA_MEM_BASE_ADDR
	mov ecx, NR_CHARS		
	shr ecx, 1			;2 chars per iteration
scr_clear_again:
	mov [eax], edx
	add eax, 4
	loop scr_clear_again

	pop edx
	pop ecx
	pop eax
	ret

;-------------------------------------------------------------------------------; scr_clear_row
; It clears the current row
;-------------------------------------------------------------------------------
scr_clear_row:
	push eax
	push ecx
	push edx

	xor eax, eax
	xor edx, edx
	mov al, SPACE_ASCII_CODE
	mov ah, [scr_char_attrib]
	mov dx, ax
	shl eax, 16
	add edx, eax


	call scr_calc_offset
	add eax, VGA_MEM_BASE_ADDR
	mov ecx, [scr_curs_x]
	shl ecx, 1
	sub eax, ecx	

	mov ecx, NR_COLS
	shr ecx, 1			;We write 2 chars (4 bytes) at a time
scr_clear_row_chars_again:
	mov [eax], edx
	add eax, 4
	loop scr_clear_row_chars_again

	pop edx
	pop ecx
	pop eax
	ret

;-------------------------------------------------------------------------------; scr_new_line
;-------------------------------------------------------------------------------
scr_new_line:
	mov dword [scr_curs_x], 0
	inc dword [scr_curs_y]
	cmp dword [scr_curs_y], NR_ROWS
	jne scr_new_line_end
	call scr_scroll_up
scr_new_line_end:
	call scr_curs_draw
	ret


;-------------------------------------------------------------------------------; scr_calc_offset
; It returns the offset in EAX (in bytes) for the cursor
; The software should be independent of the number of rows and columns
;-------------------------------------------------------------------------------
scr_calc_offset:
	push ecx	

	xor eax, eax
	mov ecx, [scr_curs_y]
	cmp ecx, 0
	jz scr_calc_offset_loop_end
scr_calc_offset_loop:
	add eax, NR_COLS
	loop scr_calc_offset_loop
scr_calc_offset_loop_end:
	add eax, [scr_curs_x]
	shl eax, 1 

	pop ecx
	ret	

;-------------------------------------------------------------------------------
; scr_putc:
; It takes the char to be printed in AL
;-------------------------------------------------------------------------------
scr_putc:
	push edx

	mov dl, al	;backup the char to be written
	call scr_calc_offset
	add eax, VGA_MEM_BASE_ADDR
	
	;Write the char
	mov dh, [scr_char_attrib]
	mov [eax], dl
	mov [eax + 1], dh

	call scr_curs_inc	

	pop edx
	ret	

;-------------------------------------------------------------------------------; scr_print
; It prints the string pointed by EAX and returns in EAX the number of
; characters actually printed.
; It also updates the cursor.
;-------------------------------------------------------------------------------
scr_print:
	push ebx
	push ecx
	push edx
	
	mov edx, eax		;Backup the pointer to char
	
	xor ecx, ecx		;ECX - counter, number of characters written
scr_print_char_again:
	call scr_calc_offset
	add eax, VGA_MEM_BASE_ADDR

	;Print the string char by char
	;EAX points to the video memory
	;EDX points to the string to be printed
	;ECX counts the number of characters
	;EBX is used as an auxiliary register
	mov bh, [scr_char_attrib]
	cmp byte [edx], 0
	je scr_print_end	;We've reached the null character
	mov bl, [edx]
	mov byte [eax], bl
	mov byte [eax + 1], bh
	inc ecx			;One more character written
	inc edx			;Point to the next character

	call scr_curs_inc
	jmp scr_print_char_again
scr_print_end:
	mov eax, ecx	;Return the number of printed chars in EAX	
	
	pop edx
	pop ecx
	pop ebx
	ret

;-------------------------------------------------------------------------------
; scr_scroll_up
; It scrolls up the screen and updates the cursor properly
;-------------------------------------------------------------------------------
scr_scroll_up:
	push eax
	push ecx
	push esi
	push edi

	mov eax, VGA_MEM_BASE_ADDR
	mov edi, eax			;EDI points to the 1st line on the scr

	mov ecx, NR_COLS
	shl ecx, 1
	add eax, ecx
	mov esi, eax			;ESI points to the 2nd line on the scr

	mov ecx, NR_CHARS
	sub ecx, NR_COLS
	shr ecx, 1
	cld
	rep
		movsd
		
	pop edi
	pop esi	
	pop ecx
	pop eax
	ret


;-------------------------------------------------------------------------------; scr_inc_curs
;-------------------------------------------------------------------------------
scr_curs_inc:
	push eax

	inc dword [scr_curs_x]
	cmp dword [scr_curs_x], NR_COLS
	jne scr_curs_inc_end
	mov dword [scr_curs_x], 0
	inc dword [scr_curs_y]
	cmp dword [scr_curs_y], NR_ROWS
	jne scr_curs_inc_end
	call scr_scroll_up
	sub dword [scr_curs_y], 1
	call scr_clear_row
scr_curs_inc_end:
	call scr_curs_draw

	pop eax
	ret


;-------------------------------------------------------------------------------
; scr_curs_draw
;-------------------------------------------------------------------------------
scr_curs_draw:
	push eax
	push ebx
	push edx
	
	mov eax, [scr_curs_y]
	shl eax, 6
	mov ebx, [scr_curs_y]
	shl ebx, 4
	add ebx, eax
	add ebx, [scr_curs_x]

	mov al, 14
	mov dx, 0x3d4
	out dx, al

	mov al, bh
	mov dx, 0x3d5
	out dx, al

	mov al, 15
	mov dx, 0x3d4
	out dx, al

	mov al, bl
	mov dx, 0x3d5
	out dx, al

	pop edx
	pop ebx
	pop eax
	ret
