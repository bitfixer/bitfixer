;
;
;       ZX Maths Routines
;
;       9/12/02 - Stefano Bodrato
;
;       $Id: deq.asm,v 1.4 2015/08/10 08:52:12 stefano Exp $
;


IF FORzx
		INCLUDE  "zxfp.def"
ENDIF
IF FORzx81
		INCLUDE  "81fp.def"
ENDIF
IF FORlambda
		INCLUDE  "lambdafp.def"
ENDIF

                PUBLIC    deq

                EXTERN	fsetup
                EXTERN	f_yesno

.deq
        call    fsetup
	defb	ZXFP_SUBTRACT
IF FORlambda
	defb	ZXFP_NOT + 128
ELSE
	defb	ZXFP_NOT
	defb	ZXFP_END_CALC
ENDIF

	jp	f_yesno