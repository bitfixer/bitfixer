;
;       Philips VG-5000 pseudo graphics routines
;	Version for the 2x3 graphics symbols
;
;       Stefano Bodrato 2015
;
;
;       Xor pixel at (x,y) coordinate.
;
;
;	$Id: xorpixl.asm,v 1.1 2015/10/09 13:02:43 stefano Exp $
;


			INCLUDE	"graphics/grafix.inc"

			PUBLIC	xorpixel

			EXTERN	div3
			EXTERN	coords
			EXTERN	base_graphics

.xorpixel
			ld	a,h
			cp	maxx
			ret	nc
			ld	a,l
			cp	maxy
			ret	nc		; y0	out of range

			ld	(coords),hl
			
			push	bc

			ld	c,a	; y
			ld	b,h	; x
			
			push	bc
			
			ld	hl,div3
			ld	d,0
			ld	e,c
			inc	e
			add	hl,de
			ld	a,(hl)
			
			srl	b	; x/2
			ld  l,b
			
			ld  b,a		; y/3
			
			and  a
			jr   z,zrow
			add  7		; bias the default scroll register settings and so on..
.zrow
			ld	h,a	; y/3

;--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
			push hl
			push bc
			call $98		; direct read access
			pop bc
			pop hl

;--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

			ex	(sp),hl		; save char position on screen <=> restore x,y  (y=h, x=l)

			
			ld	a,l

			sub	b
			sub	b
			sub	b		; we get the remainder of y/3
			
			ld	l,a
			ld	a,16		; the pixel we want to draw
			
			jr	z,iszero
			bit	0,l
			jr	nz,is1
			srl a
			srl a
.is1
			srl a
			srl a
.iszero
			
			bit	0,h
			jr	z,evenrow
			add	a,a		; move down the bit
.evenrow
			xor  d
			ld  d,a

;--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
			pop	hl
			call   $92		; direct write access
;--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
			
			pop	bc
			ret

