;
;	ZX Spectrum specific routines
;	by Stefano Bodrato, 3/07/2006
;
;	Currah uSpeech call, direct call, 
;	you must pass a string coded with allophones
;
;	int currah_direct(char *allophones);
;
;
;	$Id: currah_direct.asm,v 1.2 2015/01/19 01:33:11 pauloscustodio Exp $
;

	PUBLIC	currah_direct
	
currah_direct:
	pop	bc
	pop	hl	; text
	push	hl
	push	bc

	ld	de,65361

	ld	a,(hl)
	and	a
	ret	z

loop:	ld	a,(hl)
	and	a
	jr	nz,next
	ld	(65364),de
	ret
next:
	ld	(de),a		; load allophone in buffer
	inc	hl
	dec	de
	jr	loop
