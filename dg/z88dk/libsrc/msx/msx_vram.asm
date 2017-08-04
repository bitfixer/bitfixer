;
;	MSX specific routines
;	by Stefano Bodrato, 29/11/2007
;
;	int msx_vram();
;
;	Detects the VRAM size (in KB)
;
;	$Id: msx_vram.asm,v 1.5 2015/01/19 01:32:57 pauloscustodio Exp $
;

	PUBLIC	msx_vram
	
msx_vram:

	ld	a,(0FAFCh)		; mode
	and	@00000110		; extract VRAM size
	ld	hl,16			; assume 16K (MSX1)
	ret	z			; good assumption
	cp	@00000010		; 64K?
	ld	l,64			; assume so
	ret	z			; good assumption
	add	hl,hl			; 128K
	ret
