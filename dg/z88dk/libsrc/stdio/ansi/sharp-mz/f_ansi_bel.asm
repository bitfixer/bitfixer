;
; 	ANSI Video handling for the Sharp MZ
;	By Stefano Bodrato - 5/5/2000
;
; 	BEL - chr(7)   Beep it out
;
;
;	$Id: f_ansi_bel.asm,v 1.3 2015/01/19 01:33:19 pauloscustodio Exp $
;

	PUBLIC	ansi_BEL


.ansi_BEL
	jp	$3E	; Beep !
