/*

	MSX specific routines

	GFX - a small graphics library 
	Copyright (C) 2004  Rafael de Oliveira Jannone

	Blit - Under development
	
	$Id: msx_blit_ram_vram.c,v 1.3 2009/04/15 21:00:58 stefano Exp $
*/

#include <msx.h>

void msx_blit_ram_vram(unsigned char* source, unsigned int dest, unsigned char w, unsigned char h, int sjmp, int djmp) {
	while (h--) {
		msx_vwrite_direct(source, dest, w);
		source += sjmp;
		dest += djmp;		
	}
}
