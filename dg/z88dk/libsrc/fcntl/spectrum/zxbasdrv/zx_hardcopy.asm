;
; Just enter BASIC at the "Hardcopy" line
;
; Stefano - 12/7/2006
;
; void zx_hardcopy()
;
; $Id: zx_hardcopy.asm,v 1.4 2015/01/19 01:32:43 pauloscustodio Exp $

	PUBLIC	zx_hardcopy
	
	EXTERN	zx_goto


.zx_hardcopy
	ld	hl,7800		; BASIC routine for "hardcopy"
	jp	zx_goto

