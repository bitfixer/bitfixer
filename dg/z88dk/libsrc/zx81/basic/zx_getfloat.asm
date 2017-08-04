;
;	ZX 81 specific routines 
;	by Stefano Bodrato, 04/08/2009
;
;	Copy a variable from basic to float
;
;	float zx_getfloat(char *variable);
;
;
;	$Id: zx_getfloat.asm,v 1.3 2015/08/11 07:16:35 stefano Exp $
;

PUBLIC	zx_getfloat
EXTERN	zx_locatenum
EXTERN	fa

IF FORlambda
	INCLUDE  "lambdafp.def"
ELSE
	INCLUDE  "81fp.def"
ENDIF

; hl = char *variable

zx_getfloat:
	call	zx_locatenum
	jr	c,error

	; Move from variable to SP calculator

	ld	a,(hl)
	inc	hl
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	inc	hl
	ld	c,(hl)
	inc	hl
	ld	b,(hl)

	call	ZXFP_STK_STORE

	rst	ZXFP_BEGIN_CALC
	defb	ZXFP_END_CALC		; Now HL points to the float on the FP stack

	ld	(ZXFP_STK_PTR),hl	; update the FP stack pointer (equalise)
	
	ld	de,fa+5
	ld	b,5
.bloop2
	ld	a,(hl)
	ld	(de),a
	inc	hl
	dec	de
	djnz	bloop2

	ret


error:	; force zero
	ld	hl,fa+5
	ld	(hl),0
	ld	d,h
	ld	e,l
	dec	de
	ld	bc,5
	lddr
	ret
