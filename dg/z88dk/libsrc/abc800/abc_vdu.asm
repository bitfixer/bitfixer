;
;	ABC800 specific routines
;	by Stefano Bodrato, Oct 2007
;
;	Set video control registers
;
;	int  abc_vdu(unsigned char register, unsigned char value);
;
;
;	$Id: abc_vdu.asm,v 1.2 2015/01/19 01:32:41 pauloscustodio Exp $
;	

PUBLIC	abc_vdu

abc_vdu:
	pop	bc
	pop	de
	pop	hl
	push	hl
	push	de
	push	bc
	ld	a,l
	out	(56),a
	ld	a,e
	out	(57),a
	ret
