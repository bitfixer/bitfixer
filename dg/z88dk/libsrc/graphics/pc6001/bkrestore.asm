;
; Fast background restore
;
; PC6001 version
;
;
; $Id: bkrestore.asm,v 1.2 2015/01/19 01:32:51 pauloscustodio Exp $
;


	PUBLIC    bkrestore
	EXTERN	pixeladdress

.bkrestore

; __FASTCALL__ : sprite ptr in HL
	
	push	hl
	pop	ix

	ld	h,(ix+2) ; restore sprite position
	ld	l,(ix+3)

	ld	a,(ix+0)
	ld	b,(ix+1)
	cp	9
	jr	nc,bkrestore

._sloop
	push	bc
	push	hl
	
	ld	a,(ix+4)
	and	@10101010
	ld	(hl),a
	inc	hl
	ld	a,(ix+4)
	and	@01010101
	rla
	ld	(hl),a
	inc	hl

	ld	a,(ix+5)
	and	@10101010
	ld	(hl),a
	inc	hl
	ld	a,(ix+5)
	and	@01010101
	rla
	ld	(hl),a
	inc	hl

	inc	ix
	inc	ix

	pop	hl
	ld      bc,32      ;Go to next line
	add     hl,bc
	
	pop	bc
	djnz	_sloop
	ret

.bkrestorew
	push	bc

	ld	a,(ix+4)
	and	@10101010
	ld	(hl),a
	inc	hl
	ld	a,(ix+4)
	and	@01010101
	rla
	ld	(hl),a
	inc	hl

	ld	a,(ix+5)
	and	@10101010
	ld	(hl),a
	inc	hl
	ld	a,(ix+5)
	and	@01010101
	rla
	ld	(hl),a
	inc	hl

	ld	a,(ix+6)
	and	@10101010
	ld	(hl),a
	inc	hl
	ld	a,(ix+6)
	and	@01010101
	rla
	ld	(hl),a

	inc	ix
	inc	ix
	inc	ix

	pop	hl
	ld      bc,32      ;Go to next line
	add     hl,bc
	
	pop	bc
	djnz	bkrestorew
	ret
