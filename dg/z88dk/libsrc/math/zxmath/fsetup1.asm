;
;       ZX Maths Routines
;
;       6/12/02 - Stefano Bodrato
;
;       $Id: fsetup1.asm,v 1.4 2015/08/10 08:52:12 stefano Exp $
;



; Set up the registers for our operation
; Peeks the parameter from FA


IF FORzx
		INCLUDE  "zxfp.def"
ENDIF
IF FORzx81
		INCLUDE  "81fp.def"
ENDIF
IF FORlambda
		INCLUDE  "lambdafp.def"
ENDIF


                PUBLIC    fsetup1
		EXTERN	fa

.fsetup1

	ld	hl,fa+5

	ld	a,(hl)
	dec	hl
	ld	e,(hl)
	dec	hl
	ld	d,(hl)
	dec	hl
	ld	c,(hl)
	dec	hl
	ld	b,(hl)

	; load in the FP calculator stack	
	call	ZXFP_STK_STORE
        jp	ZXFP_BEGIN_CALC

