	INCLUDE	"graphics/grafix.inc"
	PUBLIC	setxy
	EXTERN	l_cmp

	EXTERN	coords

;
;	$Id: w_setxy.asm,v 1.3 2015/01/19 01:32:46 pauloscustodio Exp $
;

; ******************************************************************
;
; Move current pixel coordinate to (x0,y0). Only legal coordinates
; are accepted.
;
; Wide resolution (WORD based parameters) version by Stefano Bodrato
;
; Design & programming by Gunther Strube, Copyright (C) InterLogic 1995
;
; X-range is always legal (0-255). Y-range must be 0 - 63.
;
; in:  hl,de = (x,y) coordinate
;
; registers changed	after return:
;  ..bcdehl/ixiy same
;  af....../.... different
;
.setxy

		pop	bc
		pop	de
		pop	hl
		push	hl
		push	de
		push	bc

                        push    hl
                        ld      hl,maxy
                        call    l_cmp
                        pop     hl
                        ret     nc               ; Return if Y overflows

                        push    de
                        ld      de,maxx
                        call    l_cmp
                        pop     de
                        ret     c               ; Return if X overflows
                        
                        ld      (coords),hl	; store X
                        ld      (coords+2),de   ; store Y: COORDS must be 2 bytes wider

		ret
