;
;       Sprinter C Library
;
; 	ANSI Video handling for Sprinter
;
;	$Id: f_ansi_char.asm,v 1.2 2015/01/19 01:33:19 pauloscustodio Exp $
;

	PUBLIC	ansi_CHAR
	
	EXTERN	ansi_ROW
	EXTERN	ansi_COLUMN

	PUBLIC	text_cols
	PUBLIC	text_rows

	EXTERN	text_attr
	

.text_rows   defb 32
.text_cols   defb 80

; a = character to print - need to handle attributes
.ansi_CHAR
	ex	af,af
	ld	a,(ansi_ROW)
	ld	d,a
	ld	a,(ansi_COLUMN)
	ld	e,a	
	ld	a,(text_attr)
	ld	b,a
	ex	af,af
	ld	c,$58		;PUTCHAR
	rst	$10
	ret
