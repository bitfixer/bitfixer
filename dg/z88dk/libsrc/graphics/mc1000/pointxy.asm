	INCLUDE	"graphics/grafix.inc"

	PUBLIC	pointxy

	EXTERN	pixeladdress

;
;	$Id: pointxy.asm,v 1.3 2015/01/19 01:32:49 pauloscustodio Exp $
;

; ******************************************************************
;
; Check if pixel at	(x,y) coordinate is	set or not.
;
; Design & programming by Gunther Strube, Copyright (C) InterLogic 1995
;
; (0,0) origin is defined as the top left corner.
;
;  in:	hl =	(x,y) coordinate of pixel to test
; out:	Fz =	0, if pixel is set, otherwise Fz = 1.
;
; registers changed	after return:
;  ..bcdehl/ixiy same
;  af....../.... different
;
.pointxy
			IF maxx <> 256
				ld	a,h
				cp	maxx
				ret	nc
			ENDIF

				ld	a,l
				cp	maxy
				ret	nc			; y0	out of range

				push	bc
				push	de
				push	hl

				call	pixeladdress
				ld	b,a
				ld	a,1
				jr	z, test_pixel		; pixel is at bit 0...
.pixel_position	rlca
				djnz	pixel_position
.test_pixel			;ex	de,hl
				and	(hl)
				pop	hl
				pop	de
				pop	bc
				ret
