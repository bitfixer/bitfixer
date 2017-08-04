;
;	MSX extension for "GFX - a small graphics library" by Jannone
;

	INCLUDE	"graphics/grafix.inc"

	PUBLIC	surface_plotpixel

	EXTERN	surface_pixeladdress
	EXTERN	coords

;
;	$Id: surface_plotpixl.asm,v 1.5 2015/01/19 01:32:49 pauloscustodio Exp $
;

; ******************************************************************
;
; Plot pixel at (x,y) coordinate in current screen buffer.
;
; Design & programming by Gunther Strube, Copyright (C) InterLogic 1995
;
; The (0,0) origin is placed at the top left corner.
;
; in:  hl = (x,y) coordinate of pixel (h,l)
;
; registers changed after return:
;  ..bc..../ixiy same
;  af..dehl/.... different
;
.surface_plotpixel
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
				call	surface_pixeladdress
				ld	b,a
				ld	a,1
				jr	z, or_pixel		; pixel is at bit 0...
.plot_position			rlca
				djnz	plot_position
.or_pixel			ex	de,hl
				or	(hl)
				ld	(hl),a
				pop	bc
				ret
