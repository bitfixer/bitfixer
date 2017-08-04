;
;	TRS-80 (EG2000+HT1080) specific routines
;	by Stefano Bodrato, Fall 2015
;
;	int set_psg(int reg, int val);
;
;	Play a sound by PSG
;
;
;	$Id: set_psg.asm,v 1.1 2015/10/28 07:21:45 stefano Exp $
;

	PUBLIC	set_psg
	EXTERN		set_psg_callee

	EXTERN ASMDISP_SET_PSG_CALLEE
	

set_psg:

	pop	bc
	pop	de
	pop	hl

	push	hl
	push	de
	push	bc
	
	jp set_psg_callee + ASMDISP_SET_PSG_CALLEE

