;
;
;       ZX Maths Routines
;
;       8/12/02 - Stefano Bodrato
;
;       $Id: cos.asm,v 1.4 2015/08/10 08:52:12 stefano Exp $
;


;double cos(double)
;Number in FA..


IF FORzx
		INCLUDE  "zxfp.def"
ENDIF
IF FORzx81
		INCLUDE  "81fp.def"
ENDIF
IF FORlambda
		INCLUDE  "lambdafp.def"
ENDIF

                PUBLIC    cos

                EXTERN	fsetup1
                EXTERN	stkequ

.cos
        call    fsetup1
IF FORlambda
	defb	ZXFP_COS + 128
ELSE
	defb	ZXFP_COS
	defb	ZXFP_END_CALC
ENDIF
        jp      stkequ
