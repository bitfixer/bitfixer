;
;	z88dk library: Generic VDP support code
;
;	extern void vread(unsigned int source, void* dest, unsigned int count);
;
;	Transfer count bytes from VRAM to RAM
;
;	$Id: gen_vread.asm,v 1.2 2015/01/19 01:32:57 pauloscustodio Exp $
;


	PUBLIC	msx_vread
	EXTERN     SETRD
	
	INCLUDE	"msx/vdp.inc"


msx_vread:

	ld      ix,2
	add     ix,sp

	ld c, (ix+0)	; count
	ld b, (ix+1)

	ld e, (ix+2)	; dest
	ld d, (ix+3)
	
	ld l, (ix+4)	; source
	ld h, (ix+5)
	
	;ld	ix,LDIRMV
	;jp	msxbios

	call	SETRD
	ex	(sp),hl		; VDP Timing
	ex	(sp),hl		; VDP Timing

rdloop:
	in	a,(VDP_DATAIN)
	ld	(de),a
	inc	de
	dec	bc
	ld	a,c
	or	b
	jr	nz,rdloop
	ret

