;
; 	ANSI Video handling for the ABC80
;
; 	Clean a text line
;
;	Stefano Bodrato - may 2000
;
; in:	A = text row number
;
;
;	$Id: f_ansi_dline.asm,v 1.4 2015/01/19 01:33:18 pauloscustodio Exp $
;


	PUBLIC	ansi_del_line


.ansi_del_line

	ld	hl,$374		; ROW table in ROM
	ld	d,0
	rla
	ld	e,a
	add	hl,de
	ld	e,(hl)
	inc	hl
	ld	d,(hl)

	jp	$267
