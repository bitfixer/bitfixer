;
;	Sharp X1 specific routines
;   switch to 40 columns text mode
;	Uses high resolution text if available
;
;	$Id: x1_set_text_40.asm,v 1.3 2015/01/19 01:33:25 pauloscustodio Exp $
;

	PUBLIC	x1_set_text_40
;	LIB		x1_get_pcg_version
	EXTERN		set_crtc_10
	EXTERN	text_cols


t40v1:
	defb 37h, 28h, 2Dh, 34h, 1Fh, 02h, 19h, 1Ch, 00h, 07h
;t40v2:
;	defb 35h, 28h, 2Dh, 84h, 1Bh, 02h, 19h, 1Ah, 00h, 0Fh

x1_set_text_40:
;		call	x1_get_pcg_version
;		ld		a,l
;		dec		a
;		push	af
;		jr		nz,pcgv2
		ld		hl,t40v1
;		jr		set_vmode
;pcgv2:
;		ld		hl,t40v2
set_vmode:
		call	set_crtc_10
;		pop		af
;		jr		z,x1mode	; low resolution text (16khz, ...)
;		ld		a,3			; high resolution text (16khz, ...)
;x1mode:
		xor		a
		ld		bc,1FD0h
		out		(c),a
		
		ld		a,40
		ld		(text_cols),a

		ret
