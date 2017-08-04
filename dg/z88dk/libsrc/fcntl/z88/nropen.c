/*
 *      Part of the library for fcntl
 *
 *      int nropen(char *name,int access, mode_t mode)
 *
 *      djm 27/4/99
 *
 *      Access is either
 *
 *      O_RDONLY = 0
 *      O_WRONLY = 1    Starts afresh?!?!?
 *      O_APPEND = 256
 *
 *      All others are ignored(!) - i.e. mode is ignored
 *
 *	We open a file with a near filespec - good for devices
 *	since they don't have the startup code
 *
 *	djm 15/4/2000
 *
 * -----
 * $Id: nropen.c,v 1.3 2013/03/03 23:51:11 pauloscustodio Exp $
 */


#include <fcntl.h>      /* Or is it unistd.h, who knows! */

int nropen(char *name, int flags, mode_t mode, char *buf, size_t len)
{
#asm
        INCLUDE "fileio.def"
        
        ld      ix,0
        add     ix,sp
        ld      l,(ix+10)        ;lower 16 of filename
        ld      h,(ix+11)
	ld	b,0
        ld      a,(ix+8)        ;access flags
        ld      c,(ix+9)        ;top 8 of flags
        bit     0,c             ;append!
        jr      z,l_open1
        ld      a,2             ;OZ append mode-1
.l_open1
        inc     a               ;convert from UNIX mode to OZ flags
	ld	e,(ix+4)	;buf
	ld	d,(ix+5)
        ld      c,(ix+2)        ;maximum length of expanded filename
        call_oz(gn_opf)
        ld      hl,-1
        ret     c               ;open error
        push    ix              ;get filedescriptor in ix into hl
        pop     hl
#endasm
}

