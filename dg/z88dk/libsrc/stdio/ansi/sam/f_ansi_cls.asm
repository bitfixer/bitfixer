;
;       SAM Coup� C Library
;
; 	ANSI Video handling for SAM Coup�
;
; 	CLS - Clear the screen
;	
;
;	Frode Tenneb� - 29/12/2002
;
;
;	$Id: f_ansi_cls.asm,v 1.2 2015/01/19 01:33:19 pauloscustodio Exp $
;

	PUBLIC	ansi_cls

.ansi_cls
 	xor	a
	jp	$014E
