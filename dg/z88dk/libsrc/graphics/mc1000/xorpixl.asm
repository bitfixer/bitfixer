	INCLUDE	"graphics/grafix.inc"

	PUBLIC	xorpixel

	EXTERN pixeladdress
	EXTERN	coords
	EXTERN	pix_return

;
;	$Id: xorpixl.asm,v 1.4 2015/01/19 01:32:49 pauloscustodio Exp $
;

; ******************************************************************
;
; Plot pixel at (x,y) coordinate.
;
; Design & programming by Gunther Strube, Copyright (C) InterLogic 1995
;
; The (0,0) origin is placed at the bottom left corner.
;
; in:  hl	= (x,y) coordinate of pixel (h,l)
;
; registers changed	after return:
;  ..bc..../ixiy same
;  af..dehl/.... different
;
; ******************************************************************
;
; MSX Version
; XOR added by Stefano Bodrato (Feb 2001)
;
; **************
;

.xorpixel
			IF maxx <> 256
				ld	a,h
				cp	maxx
				ret	nc
			ENDIF

				ld	a,l
				cp	maxy
				ret	nc			; y0	out of range
				
				ld	(coords),hl

				push	bc
				call	pixeladdress
				ld	b,a
				ld	a,1
				jr	z, xor_pixel		; pixel is at bit 0...
.plot_position			rlca
				djnz	plot_position
.xor_pixel			;ex	de,hl
				xor	(hl)
				call	pix_return
				pop bc
				ret
