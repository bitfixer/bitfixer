;
;	Enterprise 64/128 specific routines
;	by Stefano Bodrato, 2011
;
;	set_exos_variable(unsigned char variable, unsigned char value);
;
;
;	$Id: set_exos_variable_callee.asm,v 1.4 2015/01/19 01:32:43 pauloscustodio Exp $
;


PUBLIC	set_exos_variable_callee
PUBLIC 	ASMDISP_SET_EXOS_VARIABLE_CALLEE

set_exos_variable_callee:

	pop hl
	pop de
	ex (sp),hl

; enter : e = unsigned char value
;         l = unsigned char variable

.asmentry

	ld	b,1		; SET mode
	ld	c,l		; Variable
	ld	d,e		; Value
	rst   30h
	defb  16	; SET_GET_EXOS_VARIABLE
	ld	h,0
	ld	l,d
	ret


DEFC ASMDISP_SET_EXOS_VARIABLE_CALLEE = # asmentry - set_exos_variable_callee
 
