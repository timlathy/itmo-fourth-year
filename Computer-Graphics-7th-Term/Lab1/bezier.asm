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

testtt:
  nop
  mov ah, 0xFF
  mov di, 0
  call draw_mask
  mov ah, 0xFF
  mov di, 1
  call draw_mask
  mov ah, 0xFF
  mov di, 80*3
  call draw_mask
  mov ah, 0xFF
  mov di, 80*3+1
  call draw_mask

  mov ah, 0x80
  mov di, 80*1
  call draw_mask
  mov ah, 0x04
  mov di, 80*1+1
  call draw_mask
  mov ah, 0x80
  mov di, 80*2
  call draw_mask
  mov ah, 0x04
  mov di, 80*2+1
  call draw_mask
  
  xor ah, ah
  int 0x16

  mov ax, 1 ; row
  mov cx, 3 ; col
  call flood_fill
  jmp end

prologue:
  xor ax, ax
  push ax

main_loop:
  shl ax, 4 ; ax <- 16*ctr (16 bytes per curve)
  add ax, bezier_curves
  mov word [bezier_curve_addr], ax

  call draw_bezier_curve

  pop ax
  inc ax
  push ax
  cmp ax, word [bezier_curves_cnt]
  jb main_loop

end:
  ; Wait for a single key press and terminate the program
  xor ah, ah
  int 0x16
  int 0x20

; =============================================================================
; ================================= CONSTANTS =================================
; =============================================================================

section .rodata

bezier_curves_cnt: dw 8

bezier_curves:
; M 140,133 C 107,132 98,265 55,257 24,252 39,211 57,208 74,206 62,227 62,227 66,214 58,206 41,222
dw 140,133,107,132,98,265,55,257
dw 55,257,24,252,39,211,57,208
dw 57,208,74,206,62,227,62,227
dw 62,227,66,214,58,206,41,222
; M 71,253 C 120,225 107,142 140,133
dw 71,253,120,225,107,142,140,133
; M 134,138 C 134,138 89,264 150,252
dw 134,138,134,138,89,264,150,252
; M 134,138 C 84,246 130,254 142,254 150,254 157,248 161,243
dw 134,138,84,246,130,254,142,254
dw 142,254,150,254,157,248,161,243

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
; =============================== LINE DRAWING ================================
; =============================================================================

section .text

; parameters:
; * [bezier_curve_addr] - used by cubic_bezier
draw_bezier_curve:
  fld dword [bezier_t_increment]
  fldz    ; st0 <- t(=0), st1 <- inc

  fld st0 ; cubic_bezier consumes st0 (t)
  call cubic_bezier

draw_bezier_curve_loop:
  mov ax, word [bezier_px]
  mov word [line_start_px], ax
  mov ax, word [bezier_py]
  mov word [line_start_py], ax

  fld st0 ; cubic_bezier consumes st0 (t)
  call cubic_bezier

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
; returns:
; * di - row*80 + col/8 (octet offset)
; * dx - col/8
; * cx - col%8 (bit offset)
compute_pixel_pos:
	mov dx, 80
	mul dx ; dx:ax <- row*80

	mov dx, cx
	shr dx, 3   ; dx <- col/8
	and cx, 0x7 ; cx <- col%8

	add ax, dx ; ax <- row*80 + col/8 (octet offset)
	mov di, ax ; di <- octet offset
  ret

; =============================================================================

; parameters:
; * ax - row
; * cx - col
; clobbers:
; * dx
; * di
draw_pixel:
  call compute_pixel_pos

  neg cl
  add cl, 7 ; cl <- 7-col%8

  mov ah, 1
  shl ah, cl ; ah <- 2^(7-col%8)

  jmp draw_mask

; =============================================================================

; parameters:
; * [es:di] - octet offset
; * ah - pixel mask
; clobbers:
; * dx
draw_mask:
	; Set the bit mask graphics register (index 0x08 at port 0x3ce)
  ; to the bits we want to set to 1
	mov dx, 0x3ce
  mov al, 0x8
	out dx, ax
  ; Fill the latch registers first to preserve pixels outside the mask
  ; (0 bits in the mask are taken from the latches)
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
; * [bezier_curve_addr] - address of an array of [p0x,p0y,p1x,p1y,p2x,p2y,p3x,p3y] signed 16-bit integers
; return values:
; * [bezier_px] - p(t).x
; * [bezier_py] - p(t).y 
; clobbers:
; * bx
cubic_bezier:
  mov bx, word [bezier_curve_addr]

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
  fimul word [bx] ; st0 <- (1-t)^3*p0x
  fxch st1
  fimul word [bx+2] ; st0 <- (1-t)^3*p0y
  
  fxch st2 ; st0 <- 3*(1-t)^2*t, st1 <- (1-t)^3*p0x, st2 <- (1-t)^3*p0y
  fld st0
  fimul word [bx+4] ; st0 <- 3*(1-t)^2*t*p1x, st1 <- 3*(1-t)^2*t
  fxch st1
  fimul word [bx+6] ; st0 <- 3*(1-t)^2*t*p1y, st1 <- 3*(1-t)^2*t*p1x

  faddp st3, st0 ; st0 <- 3*(1-t)^2*t*p1x, st1 <- (1-t)^3*p0x, st2 <- p(p0y, p1y)
  faddp ; st0 <- p(p0x, p1x), st1 <- p(p0y, p1y)

  fxch st2 ; st0 <- 3*(1-t)*t*t, st1 <- p(p0y, p1y), st2 <- p(p0x, p1x)
  fld st0
  fimul word [bx+8]
  fxch st1
  fimul word [bx+10] ; st0 <- 3*(1-t)*t*t*p2y, st1 <- 3*(1-t)*t*t*p2x

  faddp st2, st0 ; st0 <- 3*(1-t)*t*t*p2x, st1 <- p(p0y, p1y, p2y)
  faddp st2, st0 ; st0 <- p(p0y, p1y, p2y), st1 <- p(p0x, p1x, p2x)

  fxch st2 ; st0 <- t*t*t, st1 <- p(p0x, p1x, p2x), st2 <- p(p0y, p1y, p2y)
  fld st0
  fimul word [bx+12]
  fxch st1
  fimul word [bx+14] ; st0 <- t*t*t*p3y, st1 <- t*t*t*p3x

  faddp st3, st0 ; st0 <- t*t*t*p3x, st1 <- p(p0x, p1x, p2x), st2 <- p(p0y, p1y, p2y, p3y)
  faddp ; st0 <- p(p0x, p1x, p2x, p3x), st1 <- p(p0y, p1y, p2y, p3y)

  fistp word [bezier_px]
  fistp word [bezier_py]

  ret

; =============================================================================
; ================================= FLOOD FILL ================================
; =============================================================================

ff_leftmost_bit: db 0
ff_rightmost_bit: db 0

ff_leftmost_filled: dw 0
ff_rightmost_filled: dw 0

ff_row: dw 0
ff_octet: dw 0

ff_stack_pixel_count: dw 0

; parameters:
; * ax - starting row
; * cx - starting col
; clobbers:
; * everything
flood_fill:
  mov word [ff_stack_pixel_count], 1
  push ax
  push cx

  ; Reading from video memory should compare the color of each pixel
  ; in the octet with the boundary color and return a bitset:
  mov ax, 0x6 ; boundary color (hardcoded for now)
  call flood_fill_init_color_comp

.fill_row:
  mov bx, sp
  mov ax, word [bx+2] ; ax <- row
  mov cx, word [bx]   ; cx <- col
  call compute_pixel_pos ; di <- octet, cx <- bit, dx <- col/8
  mov word [ff_octet], di
  mov al, byte [es:di]   ; compare against the target color (0 = unfilled, 1 = filled)
  nop
  call flood_fill_find_fill_mask
  mov ah, bl ; ah <- mask
  call draw_mask

.set_lm_rm_filled_cols_to_octet_bits:
  mov bx, sp
  mov cx, word [bx] ; cx <- col
  and cl, 0xf8 ; cx <- col divisible by 8
  mov al, byte [ff_leftmost_bit]
  neg al
  add al, 7  ; al <- 7 - bit index (highest bit is lowest col)
  movzx ax, al
  add ax, cx
  mov word [ff_leftmost_filled], ax
  movzx ax, byte [ff_rightmost_bit]
  add ax, cx
  mov word [ff_rightmost_filled], ax

.fill_row_to_left:
  cmp byte [ff_leftmost_bit], 7 ; was the leftmost bit in the octet filled?
  jne .fill_row_to_right        ; if not, we don't need to fill to the left
  call flood_fill_to_left       ; ! cx should be col divisible by 8 here !
  mov word [ff_leftmost_filled], cx
  
.fill_row_to_right:
  cmp byte [ff_rightmost_bit], 0 ; was the rightmost bit in the octet filled?
  jne .pick_neighbors
  mov di, word [ff_octet]
  mov bp, sp
  mov cx, word [bp] ; cx <- col
  and cl, 0x8       ; cx <- starting col divisible by 8
  call flood_fill_to_right
  mov word [ff_rightmost_filled], cx

.pick_neighbors:
  pop ax ; remove current col
  pop ax ; ax <- row
  mov word [ff_row], ax
.scan_row_above:
  sub ax, 1
  js .scan_row_below
  call scan_row
.scan_row_below:
  mov ax, word [ff_row]
  add ax, 1
  cmp ax, 480
  je .move_next
  call scan_row

.move_next:
  sub word [ff_stack_pixel_count], 1
  jnz .fill_row
  ret

; =============================================================================

; parameters:
; * al - color comparison bitset (1 = boundary color, 0 = to fill)
; * cl - starting pixel's index (col % 8)
; returns:
; * [ff_leftmost_bit] - index of the leftmost bit
; * [ff_rightmost_bit] - index of the rightmost bit
; * bl - fill mask
; examples:
; * al = 0010 _0_011, cl = 3 => bx = 00011100, [lm] = 4, [rm] = 2
; * al = 0101 010_0_, cl = 0 => bx = 00000011, [lm] = 1, [rm] = 0
; * al = _0_001 0100, cl = 7 => bx = 11100000, [lm] = 7, [rm] = 5
flood_fill_find_fill_mask:
  mov ah, al ; ah <- copy of the resulting bitset
  mov ch, cl ; ch <- copy of the starting pixel's bit index

.leftmost_bit:
  mov bl, -1 ; bl <- inverted mask of pixels to color in the current octet
  mov cl, 8
  sub cl, ch ; cl <- 8 - starting bit
  rcl al, cl ; starting bit is in carry
.leftmost_loop:
  rcr bl, 1  ; mask <- next 0 bit
  dec cl
  jz .leftmost_found
  rcr al, 1  ; CF <- next pixel
  jnc .leftmost_loop
.leftmost_found:
  ror bl, cl ; align mask to octet
  neg cl
  add cl, 7  ; cl <- 7 - shift (leftmost bit index)
  mov byte [ff_leftmost_bit], cl

.rightmost_bit:
  mov bh, -1 ; bl <- inverted mask of pixels to color in the current octet
  mov al, ah ; al <- bitset
  mov cl, ch
  inc cl     ; cl <- bit index + 1
  rcr al, cl ; starting bit is in carry
.rightmost_loop:
  rcl bh, 1  ; mask <- next 0 bit
  dec cl
  jz .rightmost_found
  rcl al, 1  ; CF <- next pixel
  jnc .rightmost_loop
.rightmost_found:
  rol bh, cl ; align mask to octet
  mov byte [ff_rightmost_bit], cl
.invert_and_combine_masks:
  not bx    
  or bl, bh 
  ret

; =============================================================================

scan_row_row: dw 0

; parameters:
; * ax - row
; * [ff_leftmost_filled]
; * [ff_rightmost_filled]
; pushes on stack:
; * pairs of (row, col) for all rightmost pixels to be filled
; side effects:
; * increments [ff_stack_pixel_count] by the number of pairs found
; clobbers:
; * everything
scan_row:
  pop si ; si <- return address (the stack will be filled with return values)
  mov word [scan_row_row], ax
  mov cx, word [ff_rightmost_filled]
  call compute_pixel_pos ; di <- octet, cx <- bit, dx <- col/8
  shl dx, 3 ; dx <- col divisible by 8
.first_iter:
  mov al, byte [es:di]
  mov ah, 0x80
  sar ah, cl ; ah <- mask with zeroes to the right of the rightmost filled col
  and al, ah ; al <- comparison bitset with bits past col set to 0
  xor bx, bx
  jmp .check_col_against_leftmost
.loop:
  sub dx, 8  ; col <- col - 8
  js .loop_end
.load_octet:
  dec di
  mov al, byte [es:di] ; cx <- bit, dx <- col/8
.check_col_against_leftmost:
  mov cx, word [ff_leftmost_filled]
  cmp dx, cx
  ja .check_octet
  ; this octet contains the leftmost pixel, we need to mask before checking
  mov ah, 0x80
  sar ah, cl   ; ah <- mask with ones to the left of the leftmost filled col
  or al, ah    ; al <- comparison bitset with bits past lm col set to 1
  xor dx, dx   ; this is the last iteration
.check_octet:
  xor cx, cx
.loop_octet:
  bt ax, cx  ; CF <- next pixel
  jc .boundary_bit
  test bl, bl ; bl is set to 1 if the previous bit was 1
  jz .loop_octet_next
  ; if the bit is 0 but bl is 1, push the current pixel
  push word [scan_row_row] ; --- push row
  mov bx, cx
  neg bx
  add bx, 7  ; bx <- 7 - bit
  add bx, dx ; bx <- col/8*8 + 7 - bit 
  push bx    ; ----------------- push col
  xor bx, bx ; bl <- 0
  inc word [ff_stack_pixel_count]
  jmp .loop_octet_next
.boundary_bit:
  mov bl, 1
.loop_octet_next:
  inc cx
  cmp cx, 8
  je .loop
  jmp .loop_octet
.loop_end:
  jmp si

; =============================================================================

; parameters:
; * [es:di] - octet offset
; * cx - col, should be a multiple of 8
; returns:
; * cx - leftmost filled col
flood_fill_to_left:
	mov dx, 0x3ce  ; reset bit mask register so we fill the whole octet
  mov ax, 0xff08
	out dx, ax
.loop:
  sub cx, 8
  js .done
  dec di
  mov al, byte [es:di] ; compare against the target color
  test al, al
  jnz .found_boundary
  mov byte [es:di], 0xFF
  jmp .loop
.found_boundary:
  cmp al, 0xFF
  jne .find_bit_boundary
  add cx, 8  
  jmp .done
.find_bit_boundary:
  ; example: 00001000 -> 00001111, col += 3
  mov dx, cx ; dx <- col/8
  mov cx, -1 ; -1 to exclude the boundary
.octet_loop:
  inc cx
  rcr al, 1  ; CR <- rightmost pixel (boundary is on the left)
  jnc .octet_loop
  sub dx, cx
  add dx, 8  ; dx <- leftmost filled column excluding boundary
.make_octet_mask:
  mov ah, 1
  shl ah, cl
  sub ah, 1 ; ah <- fill mask
  push dx
  call draw_mask
  pop cx ; cx <- leftmost filled col
.done:
  ret

; =============================================================================

; parameters:
; * [es:di] - octet offset
; * cx - col, should be a multiple of 8
; returns:
; * cx - rightmost filled col
flood_fill_to_right:
	mov dx, 0x3ce  ; reset bit mask register so we fill the whole octet
  mov ax, 0xff08
	out dx, ax
.loop:
  add cx, 8
  cmp cx, 640
  je .done
  inc di
  mov al, byte [es:di] ; compare against the target color
  test al, al
  jnz .found_boundary
  mov byte [es:di], 0xFF
  jmp .loop
.found_boundary:
  cmp al, 0xFF
  jne .find_bit_boundary
  sub cx, 8  
  jmp .done
.find_bit_boundary:
  ; example: 00001000 -> 11111000, col += 4
  mov dx, cx ; dx <- col/8
  mov cx, -1 ; -1 to exclude the boundary
.octet_loop:
  inc cx
  rcl al, 1  ; CR <- leftmost pixel (boundary is on the right)
  jnc .octet_loop
  add dx, cx ; dx <- rightmost filled column excluding boundary
.make_octet_mask:
  mov ah, 0x80
  sar ah, cl   ; ah <- fill mask (includes the boundary here...)
  push dx
  call draw_mask
  pop cx ; cx <- leftmost filled col
.done:
  ret

; =============================================================================

; parameters:
; * ax - boundary color
; clobbers:
; * dx
flood_fill_init_color_comp:
  ; Initialize color compare register (0x02 at 0x3ce) with the boundary color
  mov dx, 0x3ce
  mov ah, al
  mov al, 0x2
  out dx, ax

  ; Read graphics mode register (0x05 at 0x3ce)
  mov al, 0x5
  out dx, al
  inc dx
  in al, dx

  or al, 0b1000 ; read mode (bit 3) = compare between display memory and reg 0x2

  ; Save the updated graphics mode
  mov dx, 0x3ce
  mov ah, al
  mov al, 0x5
  out dx, ax

  ret
