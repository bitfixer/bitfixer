;
;	Sharp OZ family functions
;
;	ported from the OZ-7xx SDK by by Alexander R. Pruss
;	by Stefano Bodrato - Oct. 2003
;
;	Serial libraries
;	serial control commands
;
; ------
; $Id: ozsetbaud.asm,v 1.2 2015/01/19 01:33:04 pauloscustodio Exp $
;

	PUBLIC	ozsetbaud


ozsetbaud:
        ld hl,2
        add hl,sp
        in a,(43h)
        ld b,a
        or 80h
        out (43h),a
        ld a,(hl)
        out (40h),a
        inc hl
        ld a,(hl)
        out (41h),a
        ld a,b
        and 7fh
        out (43h),a
        ret
