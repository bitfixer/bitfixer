/*
 *      New stdio functions for Small C+
 *
 *      djm 4/5/99
 *
 * --------
 * $Id: ungetc.c,v 1.2 2003/10/01 20:00:16 dom Exp $
 */

#define ANSI_STDIO

#define STDIO_ASM

#include <stdio.h>

int ungetc(int c, FILE *fp)
{
#ifdef Z80
#asm
	pop	hl	;ret
	pop	de	;fp
	pop	bc	;c
	push	bc
	push	de
	push	hl
	ld	hl,-1	;EOF
	inc	de
	inc	de
	ld	a,(de)	;fp_flags
	ld	b,a
	and	_IOUSE
	ret	z	;not being used
	ld	a,b
	and	_IOEOF |_IOWRITE
	ret	nz	;cant push back after EOF (or for write stream)
	inc	de
	ld	a,(de)	;fp_ungetc
	and	a
	ret	nz
	ld	a,c
	ld	(de),a	;store the char
	ld	l,c			;return it
	ld	h,0
#endasm
#else
        if (fp == 0 || c == EOF || fp->ungetc || fp->flags&_IOWRITE ) return(EOF);
        
        fp->ungetc=(unsigned char)c;
        return(c);
#endif
}
