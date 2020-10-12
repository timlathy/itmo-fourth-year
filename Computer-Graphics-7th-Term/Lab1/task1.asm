; vim: syntax=nasm
section .data

section .text
org 0x100

; Set VGA mode to 0x12 (640x480, 16 colors)
mov ax, 0x12
int 0x10

; To control the color, we need to specify which bitplanes to write to —
; the color (0b1101) is loaded into the map mask register.
; We need to output the register index byte (0x2 for map mask) to port 0x3c4,
; then output the value byte (our color) to port 0x3c5.
; This can be accomplished with a single OUT instruction due to the fact
; that these are 8-bit ports, and 16-bit writes will be broken into two 8-bit ones.
; We need to mind the endianness: `mov ax, 0xfeff` followed by `out dx, ax` writes
; 0xfe to dx+1 and 0xff to dx.

mov dx, 0x3c4
mov ax, 0x0d02 ; 0b1101 = 0xd
out dx, ax

mov ax, 0xa000 ; video memory start
mov es, ax

; Writing 0x3 to address [A000:A2] draws two pixels, at (w,h) (22,2) and (23,2).
; Here's how it works:
; There are 640 pixels in a row, with 8 pixels per byte, 640 / 8 = 80 bytes per row
; Pixel octet offset = row * 80 + col / 8
; 0xa2 = 16 / 2 + 2 * 80 = second row, columns 23-16
; Within the octet, 3 = 0b00000011 = columns 22 and 23
mov al, 0x3
mov bx, 0xa2
mov [es:bx], al

xor ah, ah ; read key
int 0x16

int 0x20 ; terminate program

