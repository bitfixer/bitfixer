;
;	Sharp OZ family functions
;
;	ported from the OZ-7xx SDK by by Alexander R. Pruss
;	by Stefano Bodrato - Oct. 2003
;
;	Serial libraries
;
;
; ------
; $Id: ozserinton.asm,v 1.3 2015/01/19 01:33:04 pauloscustodio Exp $
;

	PUBLIC	ozserinton

	EXTERN	ozcustomisr
	EXTERN	serial_hook
	EXTERN	rxxoff_hook

	EXTERN	ozintwait
	EXTERN	serial_check_hook
	
	EXTERN	serial_int
	EXTERN	serial_int_check
	EXTERN	rxxoff_handler

ozserinton:
        ld      hl,serial_int
        ld      (serial_hook+1),hl
        ld      hl,serial_int_check
        ld      (serial_check_hook+1),hl
        ld      hl,rxxoff_handler
        ld      (rxxoff_hook+1),hl
        in a,(7)
        and 0ffh-4
        out (7),a
        ld a,1
        out (41h),a
        ret
