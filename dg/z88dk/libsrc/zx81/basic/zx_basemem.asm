;
;	ZX 81 specific routines
;	by Stefano Bodrato, 25/07/2008
;
;	int zx_basemem;
;
;	This function returns the base memory size in kbytes
;	Measures only up to the RAMPTOP limit
;
;	$Id: zx_basemem.asm,v 1.2 2015/01/19 01:33:26 pauloscustodio Exp $
;

	PUBLIC	zx_basemem
	
zx_basemem:
	ld	a,(16389)
	srl	a
	inc	a
	srl	a
	sub	16	; subtract the ROM size
	ld	l,a
	ld	h,0
	ret
