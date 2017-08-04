;
;
;       ZX Maths Routines
;
;       7/12/02 - Stefano Bodrato
;
;       $Id: amin.asm,v 1.6 2015/08/10 08:52:12 stefano Exp $
;


;double amin (double x,double y)  
;
;returns the smaller of x and y


IF FORzx
		INCLUDE  "zxfp.def"
ENDIF
IF FORzx81
		INCLUDE  "81fp.def"
ENDIF
IF FORlambda
		INCLUDE  "lambdafp.def"
ENDIF

                PUBLIC    amin

                EXTERN	fsetup
                EXTERN	stkequ

.amin
	call    fsetup
	defb	ZXFP_NO_LESS		; Not lesser
	defb	ZXFP_JUMP_TRUE		; Don't exchange
	defb	2			; [offset to go over the next byte]
	defb	ZXFP_EXCHANGE
	defb	ZXFP_END_CALC
	call	ZXFP_STK_FETCH		; take away the bigger no from stack

        jp      stkequ
