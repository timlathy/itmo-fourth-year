; vim: syntax=nasm
section .text
org 0x100

init_mode:
  ; Set VGA mode to 0x12 (640x480, 16 colors)
  mov ax, 0x12
  int 0x10

init_es:
  ; Set segment register ES to the start of video memory
  mov ax, 0xa000
  mov es, ax

init_color:
  ; Write color (0b0110 = 0x6 = brown/orange) to the map mask sequencer register
  ; (index 0x02 at port 0x3c4)
  mov dx, 0x3c4
  mov ax, 0x0602
  out dx, ax

draw_loop_prologue:
  fld dword [bezier_t_increment]
  fldz                           ; st0 <- t(=0), st1 <- inc

draw_loop:
  fst dword [bezier_t]
  call quadratic_bezier
  call draw_pixel
  fadd st0, st1
  fld1
  fcomp st0, st1
  fnstsw ax
  test ah, 0x45
  jz draw_loop  ; jump if st0 (1) > st1 (t)

end:
  ; Wait for a single key press and terminate the program
  xor ah, ah
  int 0x16
  int 0x20

draw_pixel:
  mov ax, word [bezier_py]
	mov dx, 80
	mul dx ; dx:ax <- row*80

  mov cx, word [bezier_px]
	mov dx, cx
	shr dx, 3   ; dx <- col/8
	and cx, 0x7 ; cx <- col%8

	add ax, dx ; ax <- row*80 + col/8 (octet offset)
	mov di, ax ; di <- octet offset

  neg cl
  add cl, 7 ; cl <- 7-col%8

  mov ah, 1
  shl ah, cl ; ah <- 2^(7-col%8)

	; Set the bit mask graphics register (index 0x08 at port 0x3ce)
  ; to the bits we want to set to 1
	mov dx, 0x3ce
  mov al, 0x08
	out dx, ax
	mov byte [es:di], 0xff

  ret

; p(t) = (1.0f-t)*(1.0f-t)*(1.0f-t)*p0 + 3*(1.0f-t)*(1.0f-t)*t*p1 + 3*(1.0f-t)*t*t*p2 + t*t*t*p3 
; parameters:
; * [bezier_curve] - an array of [p0x,p0y,p1x,p1y,p2x,p2y,p3x,p3y] signed 16-bit integers
; return values:
; * [bezier_px] - p(t).x
; * [bezier_py] - p(t).y 
quadratic_bezier:
  fld dword [bezier_t]
  fld1
  fsub st0, st1 ; st0 <- (1-t), st1 <- t
  fld st0       ; st0, st1 <- (1-t), st2 <- t
  fmul st0, st0 ; st0 <- (1-t)^2, st1 <- (1-t), st2 <- t
  fld st1       ; st0 <- (1-t), st1 <- (1-t)^2, st2 <- (1-t), st3 <- t
  fmul st0, st1 ; st0 <- (1-t)^3, st1 <- (1-t)^2, st2 <- (1-t), st3 <- t
  
  fld dword [bezier_3f]
  fmul st2, st0  ; st0 <- 3, st1 <- (1-t)^3, st2 <- 3*(1-t)^2
  fmulp st3, st0 ; st0 <- (1-t)^3, st1 <- 3*(1-t)^2, st2 <- 3*(1-t)

  fld st3        ; st0 <- t
  fmul st2, st0  ; st1 <- (1-t)^3, st2 <- 3*(1-t)^2*t
  fmul st0, st0  ; st0 <- t*t
  fmul st3, st0  ; st3 <- 3*(1-t)*t*t
  fmulp st4, st0 ; st0 <- (1-t)^3, st1 <- 3*(1-t)^2*t, st2 <- 3*(1-t)*t*t, st3 <- t*t*t

  fld st0 ; st0, st1 <- (1-t)^3
  fimul word [bezier_curve] ; st0 <- (1-t)^3*p0x
  fxch st1
  fimul word [bezier_curve+2] ; st0 <- (1-t)^3*p0y
  
  fxch st2 ; st0 <- 3*(1-t)^2*t, st1 <- (1-t)^3*p0x, st2 <- (1-t)^3*p0y
  fld st0
  fimul word [bezier_curve+4] ; st0 <- 3*(1-t)^2*t*p1x, st1 <- 3*(1-t)^2*t
  fxch st1
  fimul word [bezier_curve+6] ; st0 <- 3*(1-t)^2*t*p1y, st1 <- 3*(1-t)^2*t*p1x

  faddp st3, st0 ; st0 <- 3*(1-t)^2*t*p1x, st1 <- (1-t)^3*p0x, st2 <- p(p0y, p1y)
  faddp ; st0 <- p(p0x, p1x), st1 <- p(p0y, p1y)

  fxch st2 ; st0 <- 3*(1-t)*t*t, st1 <- p(p0y, p1y), st2 <- p(p0x, p1x)
  fld st0
  fimul word [bezier_curve+8]
  fxch st1
  fimul word [bezier_curve+10] ; st0 <- 3*(1-t)*t*t*p2y, st1 <- 3*(1-t)*t*t*p2x

  faddp st2, st0 ; st0 <- 3*(1-t)*t*t*p2x, st1 <- p(p0y, p1y, p2y)
  faddp st2, st0 ; st0 <- p(p0y, p1y, p2y), st1 <- p(p0x, p1x, p2x)

  fxch st2 ; st0 <- t*t*t, st1 <- p(p0x, p1x, p2x), st2 <- p(p0y, p1y, p2y)
  fld st0
  fimul word [bezier_curve+12]
  fxch st1
  fimul word [bezier_curve+14] ; st0 <- t*t*t*p3y, st1 <- t*t*t*p3x

  faddp st3, st0 ; st0 <- t*t*t*p3x, st1 <- p(p0x, p1x, p2x), st2 <- p(p0y, p1y, p2y, p3y)
  faddp ; st0 <- p(p0x, p1x, p2x, p3x), st1 <- p(p0y, p1y, p2y, p3y)

  fistp word [bezier_px]
  fistp word [bezier_py]

  ret

section .data

bezier_3f: dd 3.0
bezier_curve: dw 79,237,181,97,305,371,400,250

bezier_t_increment: dd 0.01

bezier_t: dd 0.7
bezier_px: dw 0
bezier_py: dw 0
