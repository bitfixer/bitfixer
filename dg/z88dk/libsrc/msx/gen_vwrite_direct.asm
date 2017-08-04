;
;	z88dk library: Generic VDP support code
;
;	extern void msx_vwrite_direct(void *source, u_int dest, u_int count)
;
;	Transfer count bytes from RAM (current memory page) to VRAM
;
;	$Id: gen_vwrite_direct.asm,v 1.2 2015/01/19 01:32:57 pauloscustodio Exp $
;


	PUBLIC	msx_vwrite_direct
	EXTERN     SETWRT
	
	INCLUDE	"msx/vdp.inc"


msx_vwrite_direct:

	ld      ix,2
	add     ix,sp

	ld c, (ix+0)	; count
	ld b, (ix+1)

	ld l, (ix+2)	; dest
	ld h, (ix+3)

	ld e, (ix+4)	; source
	ld d, (ix+5)

	call	SETWRT

wrtloop:
	ld	a,(de)
	out	(VDP_DATA),a
	inc	de
	dec	bc
	ld	a,c
	or	b
	jr	nz,wrtloop
	ret

