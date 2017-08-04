
	PUBLIC	pointxy

	EXTERN	coords

;
;	$Id: pointxy.asm,v 1.3 2015/01/19 01:32:49 pauloscustodio Exp $
;

; ******************************************************************
;
; Get pixel at (x,y) coordinate.
;
; Sharp MZ version.
;
; 80x50 rez.
;
;
.pointxy
				ld	a,h
				cp	80
				ret	nc
				ld	a,l
				;cp	maxy
				cp	50
				ret	nc		; y0	out of range

				push	bc
				push	de
				push	hl			

				ld	(coords),hl

				;push	bc

				ld	c,a
				ld	b,h

				push	bc

				srl	b
				srl	c
				ld	hl,$D000
;				inc	hl
				ld	a,c
				ld	c,b	; !!
				and	a
				jr	z,r_zero
				ld	b,a
				ld	de,40
.r_loop
				add	hl,de
				djnz	r_loop
.r_zero						; hl = char address
				ld	e,c
				add	hl,de

				ld	a,(hl)		; get current symbol
				;sub	$f0
				and	$f

				ex	(sp),hl		; save char address <=> restore x,y

				ld	b,a
				ld	a,1		; the bit we want to draw

				bit	0,h
				jr	z,iseven
				add	a,a		; move right the bit

.iseven
				bit	0,l
				jr	z,evenrow
				add	a,a
				add	a,a		; move down the bit
.evenrow
				and	b
		
				pop	bc
				
				pop	hl
				pop	de
				pop	bc
			
				ret
