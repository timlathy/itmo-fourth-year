; vim: syntax=nasm
section .data

section .text
org 0x100

; Set VGA mode to 0x12 (640x480, 16 colors)
mov ax, 0x12
int 0x10

; Write color (0b0110 = 0x6 = brown/orange) to the map mask sequencer register
; (index 0x02 at port 0x3c4)
mov dx, 0x3c4
mov ax, 0x0602
out dx, ax

mov ax, 240 ; row
mov cx, 313 ; starting col
mov bx, 327 ; line width
call draw_line

; Wait for a single key press and terminate the programm
xor ah, ah
int 0x16
int 0x20

; parameters:
; * ax - row, 0 to 480
; * cx - starting col, 0 to 640
; * bx - line width, 0 to 640-dx
draw_line:
	test bx, bx ; line width = 0 is a noop
	jz draw_line_end

	mov dx, 80
	mul dx ; dx:ax <- row*80

	mov dx, cx
	add dx, 7
	shr dx, 3   ; dx <- (col+7) / 8 (rounding up)
	and cx, 0x7 ; cx <- col % 8

	add ax, dx ; ax <- row*80 + col/8 (octet offset)
	mov di, ax ; di <- octet offset
	
; If the starting column is not divisible by 8,
; we need to fill the higher bits of the preceding octet first
draw_line_col_rem:
	test cx, cx
	jz draw_line_loop_prologue
	neg cx
	add cx, 8 ; cx <- 8 - col%8
	cmp cx, bx
	jbe draw_line_rem_lte_width
draw_line_rem_gt_width:
	mov cx, bx ; cx <- min(rem, line width)
draw_line_rem_lte_width:
	sub bx, cx ; bx  <- line width - rem
	mov ax, cx ; ax <- rem

	; Set the bit mask graphics register (index 0x08 at port 0x3ce)
  ; to the bits we want to set to 1
	mov dx, 0x3ce
	shl ax, 8
  or ax, 0x0008
	out dx, ax
	mov byte [es:di-1], 0xff

draw_line_loop_prologue:
	lea cx, [bx] ; cx <- line width / 8 (number of all-1s octets)
	shr cx, 3

	and bx, 0x7 ; bx <- line width % 8 (1s set in the last octet)

	mov ax, 0xa000
	mov es, ax ; es <- start of video memory

	test cx, cx
	jz draw_line_rem

	; Set the bit mask register (0x08 at 0x3ce) to 0xff (all bits will be set)
	mov dx, 0x3ce
	mov ax, 0xff08
	out dx, ax
	mov byte [es:di-1], 0xff

draw_line_loop:
	cld ; stosb increments di
	mov al, 0xff
	rep stosb

draw_line_rem:
	; Turn (line width % 8) into a bit mask: 2^(line width % 8) - 1
	mov cl, bl
	mov bx, 1
	shl bx, cl
	sub bx, 1
	; Reverse the bits in the mask
	xor ax, ax
	draw_line_rem_reverse_mask:
	rcr bl, 1
	rcr ax, 1
	sub cl, 1
	jnz draw_line_rem_reverse_mask
	; Feed it into the bit mask register (0x08 at 0x3ce)
	; Note that the reversed bits start from ah so we don't need to shift ax left
	mov dx, 0x3ce
	or ax, 0x0008
	out dx, ax
	mov byte [es:di], 0xff

draw_line_end:
	ret
