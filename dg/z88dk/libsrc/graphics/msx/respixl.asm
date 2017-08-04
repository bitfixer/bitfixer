	INCLUDE	"graphics/grafix.inc"

	PUBLIC	respixel

	EXTERN pixeladdress
	EXTERN	coords
	EXTERN	pix_return

;
;	$Id: respixl.asm,v 1.7 2015/01/19 01:32:49 pauloscustodio Exp $
;

; ******************************************************************
;
; Reset pixel at (x,y) coordinate
;
; Design & programming by Gunther Strube, Copyright (C) InterLogic 1995
;
; in:  hl = (x,y) coordinate of pixel (h,l)
;
; registers changed	after return:
;  ..bc..../ixiy same
;  af..dehl/.... different
;
.respixel
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
				jr	z, reset_pixel
.reset_position			rlca
				djnz	reset_position
.reset_pixel			;ex	de,hl
				cpl
				and	(hl)
				jp	pix_return
