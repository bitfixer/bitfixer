	INCLUDE "graphics/grafix.inc"
	PUBLIC	drawb
	EXTERN	w_pixeladdress
;	LIB	l_cmp

;
;	$Id: drawb.asm,v 1.2 2015/01/19 01:32:49 pauloscustodio Exp $
;

; ***********************************************************************
;
; drawbox Timex hires version
;
.PIXEL
	DEFB	0
.drawb
		ld ix,0
		add ix,sp
		ld l,(ix+8)
		ld h,(ix+9); x
		ld a,1	; 512... range checking needs to be fixed to 320
		cp h
		ret c
		ld e,(ix+6); y
		ld a,maxy
		cp e
		ret c
; left vertical
		call pixel_addr
		call vertical

; right vertical line
		ld l,(ix+8)
		ld h,(ix+9); x
		ld c,(ix+4) 
		ld b,(ix+5); width
		add hl,bc
		ld a,1
		cp h
		jr c,next
		ld e,(ix+6)
		call pixel_addr
		call vertical

.next
; upper horizontal line
		ld l,(ix+8)
		ld h,(ix+9) ; x
		ld e,(ix+6) ; y
		call pixel_addr
		call horizontal

.next2
; bottom horizontal line
		ld l,(ix+8)
		ld h,(ix+9);x
		ld e,(ix+6);y
		ld a,(ix+2);height
		add a,e
		ret c
		ld e,a
		ld a,maxy
		cp e
		ret c

		call pixel_addr
		; jp horizontal
.horizontal
		ld c,(ix+4)
		ld b,(ix+5) ; width

.loop3
		ld a,(de)
		or (hl)
		ld (de),a
		call incx
		ret c
		ld a,b
		or c
		ret z
		dec bc
		jr loop3
		ret


; (hl) mask
; de - screen address
.incx
		rrc (hl)
		ret nc
		bit 5,d
		jr nz,first
		set 5,d
		or a
		ret
.first
		res 5,d
		inc e
		ld a,e
		and $1f
		ret nz
		scf
		ret

.incy
		inc d
		ld a,d
		and $07
		ret nz

		ld a,d
		sub $08
		ld d,a
		ld a,e
		add a,$20
		ld e,a
		ret nc

		ld a,d
		add a,$08
		ld d,a

		and 95
		cp $58
		ccf
		ret

.pixel_addr
		ld d,0
		call w_pixeladdress
		ld b,a
		ld a,1
		jr z,getout; pixel is at bit 0...
.loop1
		rlca
		djnz loop1
.getout
		ld (PIXEL),a
		ld hl,PIXEL
		ret

.vertical
; vertical line
		ld b,(ix+2) ; height
.loop2
		ld a,(de)
		or (hl)
		ld (de),a
		call incy
		ret c
		djnz loop2
		ret
