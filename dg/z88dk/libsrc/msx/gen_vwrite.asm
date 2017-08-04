;
;	z88dk library: Generic VDP support code
;
;	extern void msx_vwrite(void *source, u_int dest, u_int count)
;
;	Transfer count bytes from RAM to VRAM (BIOS paged version)
;
;	$Id: gen_vwrite.asm,v 1.2 2015/01/19 01:32:57 pauloscustodio Exp $
;


	PUBLIC	msx_vwrite
	EXTERN     LDIRVM
	
	INCLUDE	"msx/vdp.inc"


msx_vwrite:

	ld      ix,2
	add     ix,sp

	ld c, (ix+0)	; count
	ld b, (ix+1)

	ld e, (ix+2)	; dest
	ld d, (ix+3)
	
	ld l, (ix+4)	; source
	ld h, (ix+5)
	
	jp	LDIRVM
	
