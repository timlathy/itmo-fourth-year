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

call draw_bezier_curve

end:
  ; Wait for a single key press and terminate the program
  xor ah, ah
  int 0x16
  int 0x20

; =============================================================================
; ================================= CONSTANTS =================================
; =============================================================================

section .rodata

bezier_curve: dw 79,237,181,97,305,371,400,250

bezier_3f: dd 3.0
bezier_t_increment: dd 0.01

; =============================================================================
; ==================================== VAR ====================================
; =============================================================================

section .data

bezier_curve_addr: dw 0

bezier_px: dw 0
bezier_py: dw 0

line_start_px: dw 0
line_start_py: dw 0

; =============================================================================
; ================================ SUBROUTINES ================================
; =============================================================================

section .text

; parameters:
; * [bezier_curve] - used by quadratic_bezier
draw_bezier_curve:
  fld dword [bezier_t_increment]
  fldz    ; st0 <- t(=0), st1 <- inc

  fld st0 ; quadratic_bezier consumes st0 (t)
  call quadratic_bezier

draw_bezier_curve_loop:
  mov ax, word [bezier_px]
  mov word [line_start_px], ax
  mov ax, word [bezier_py]
  mov word [line_start_py], ax

  fld st0 ; quadratic_bezier consumes st0 (t)
  call quadratic_bezier

  call draw_line

  fadd st0, st1
  fld1
  fcomp st0, st1
  fnstsw ax
  test ah, 0x45
  jz draw_bezier_curve_loop ; jump if st0 (1) > st1 (t)

; =============================================================================

; parameters:
; * ax - row
; * cx - col
; clobbers:
; * dx
; * di
draw_pixel:
	mov dx, 80
	mul dx ; dx:ax <- row*80

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
  mov al, 0x8
	out dx, ax

  mov al, byte [es:di]
	mov byte [es:di], 0xff

  ret

; =============================================================================

; parameters:
; * ax - value
; returns:
; * ax - abs(value)
; clobbers:
; * dx
abs_ax:
  mov dx, ax
  sar dx, 15
  xor ax, dx
  sub ax, dx
  ret

; =============================================================================

sign_x: dw 0
sign_y: dw 0
line_dx: dw 0
line_dy: dw 0
line_err: dw 0

; Draws a line from line_start_p{x,y} to bezier_p{x,y}
; using Bresenham's algorithm
draw_line:
  mov bx, word [line_start_px]
  mov ax, word [bezier_px]
  sub ax, bx
  jbe draw_line_x0gtex1
draw_line_x0ltx1:
  mov word [sign_x], 1
  jmp draw_line_dx
draw_line_x0gtex1:
  mov word [sign_x], -1
draw_line_dx:
  call abs_ax ; ax <- abs(x1-x0)
  mov word [line_dx], ax
  
  mov cx, word [line_start_py]
  mov ax, word [bezier_py]
  sub ax, cx
  jbe draw_line_y0gtey1
draw_line_y0lty1:
  mov word [sign_y], 1
  jmp draw_line_dy
draw_line_y0gtey1:
  mov word [sign_y], -1
draw_line_dy:
  call abs_ax
  neg ax      ; ax <- -abs(y1-y0)
  mov word [line_dy], ax

  add ax, word [line_dx]
  mov word [line_err], ax

draw_line_loop:
  mov ax, word [line_start_py]
  mov cx, word [line_start_px]
  call draw_pixel
  
  mov cx, word [line_start_px] ; cx <- line_start_px
  mov dx, word [line_start_py] ; dx <- line_start_py
  cmp cx, word [bezier_px]
  jne draw_line_loop_cont
  cmp dx, word [bezier_py]
  je draw_line_end

draw_line_loop_cont:
  mov ax, word [line_err]
  shl ax, 1 ; ax <- err*2

draw_line_2egtedy:
  mov bx, word [line_dy]
  cmp ax, bx
  jl draw_line_2eltedx
  shr ax, 1  ; ax <- err
  add bx, ax ; bx <- err + dy
  mov word [line_err], bx
  add cx, word [sign_x] ; cx <- line_start_px + sign_x
  mov word [line_start_px], cx
  shl ax, 1  ; ax <- err*2

draw_line_2eltedx:
  mov bx, word [line_dx]
  cmp ax, bx
  jg draw_line_loop
  add bx, word [line_err] ; bx <- err + line_dx (can't use ax because err could've been altered by 2egtedy!)
  mov word [line_err], bx
  add dx, word [sign_y] ; dx <- line_start_py + sign_y
  mov word [line_start_py], dx
  
  jmp draw_line_loop
  
draw_line_end:
  ret

; =============================================================================

; p(t) = (1.0f-t)*(1.0f-t)*(1.0f-t)*p0 + 3*(1.0f-t)*(1.0f-t)*t*p1 + 3*(1.0f-t)*t*t*p2 + t*t*t*p3 
; parameters:
; * fpu st0 - t
; * [bezier_curve] - an array of [p0x,p0y,p1x,p1y,p2x,p2y,p3x,p3y] signed 16-bit integers
; return values:
; * [bezier_px] - p(t).x
; * [bezier_py] - p(t).y 
quadratic_bezier:
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
