;
;	MSX specific routines
;
;	GFX - a small graphics library 
;	Copyright (C) 2004  Rafael de Oliveira Jannone
;
;	extern bool __FASTCALL__ msx_set_mode(unsigned char id);
;
;	set screen mode
;
;	$Id: msx_set_mode.asm,v 1.3 2015/01/19 01:32:57 pauloscustodio Exp $
;


	PUBLIC	msx_set_mode
	
	EXTERN	msxbios

        INCLUDE "msxbios.def"


msx_set_mode:

	push	hl
	pop	ix
	jp	msxbios

	;_init_sprites();
