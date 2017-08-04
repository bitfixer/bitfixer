;
;	Sharp OZ family functions
;
;	ported from the OZ-7xx SDK by by Alexander R. Pruss
;	by Stefano Bodrato - Oct. 2003
;
;
;	display blanking control functions
;	LCDStatus is 0c024h in the OS;
;
;	ozslow is same than ozunblankscreen
;
;
; ------
; $Id: ozfast.asm,v 1.2 2015/01/19 01:33:02 pauloscustodio Exp $
;

	PUBLIC	ozfast
	
	EXTERN	ozblankscreen
	
ozfast:
        call    ozblankscreen
        xor     a
        out     (20h),a
        out     (24h),a
        ret
