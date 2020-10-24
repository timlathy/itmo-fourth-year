; vim: syntax=nasm
section .data

section .text
org 0x100

; Set VGA mode to 0x12 (640x480, 16 colors)
mov ax, 0x12
int 0x10

; Write color (0b0110 = 0x6 = brown/orange) to the map mask register (0x02)
mov dx, 0x3c4
mov ax, 0x0602
out dx, ax

; Set segment register ES to the start of video memory
mov ax, 0xa000
mov es, ax

; 640 pixels in a row, with 8 pixels per byte, 640 / 8 = 80 bytes per row
; Octet offset = row * 80 + col / 8 = (320 * 80) + (200 / 8) = 25625 = 0x6419
; Within the octet, column 200 = 0b00000001
mov al, 0x1
mov bx, 0x6419
mov [es:bx], al

; Wait for a single key press and terminate the programm
xor ah, ah
int 0x16
int 0x20
