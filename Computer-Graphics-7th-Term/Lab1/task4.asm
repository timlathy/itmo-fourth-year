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

mov ax, 121 ; row
mov cx, 320 ; col
mov bx, 239 ; line height
call draw_line

; Wait for a single key press and terminate the programm
xor ah, ah
int 0x16
int 0x20

; parameters:
; * ax - row, 0 to 480
; * cx - col, 0 to 640
; * bx - line height, 0 to 480-ax
draw_line:
	test bx, bx ; line height = 0 is a noop
	jz draw_line_end

	mov dx, 80
	mul dx ; dx:ax <- row*80

	mov dx, cx
	shr dx, 3   ; dx <- col / 8
	and cx, 0x7 ; cx <- col % 8

	mov di, ax
	add di, dx ; di <- row*80 + col/8 (octet offset)
	
	; Turn col%8 into a bit index -- 7-2^(col%8)
	mov dx, 1
	shl dx, cl
	mov ax, 7
	sub ax, dx
	; Feed it into the bit mask graphics register (index 0x08 at port 0x3ce):
	mov dx, 0x3ce
	shl ax, 8
  or ax, 0x0008
	out dx, ax

	mov ax, 0xa000
	mov es, ax ; es <- start of video memory

draw_line_loop:
	mov byte [es:di], 0xff
	add di, 640/8 ; line offset
	sub bx, 1
	jnz draw_line_loop

draw_line_end:
	ret
