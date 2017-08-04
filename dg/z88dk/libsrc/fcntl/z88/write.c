/*
 *      Part of the library for fcntl
 *
 *      size_t write(int fd,void *ptr, size_t len)
 *
 *      djm 27/4/99
 *
 *      Close a file descriptor, pretty well identical to
 *      fclose from the other stuff
 *
 * -----
 * $Id: write.c,v 1.3 2013/03/03 23:51:11 pauloscustodio Exp $
 */


#include <sys/types.h>
#include <fcntl.h>


size_t write(int fd, void *ptr, size_t len)
{
#asm
        INCLUDE         "fileio.def"

        ld      ix,2
        add     ix,sp
        ld      c,(ix+0)        ;len
        ld      b,(ix+1)
        ld      l,(ix+2)        ;ptr
        ld      h,(ix+3)
        ld      e,(ix+4)        ;fd
        ld      d,(ix+5)
        push    de
        pop     ix
        push    bc
        ld      de,0            ;move from memory to file (de=0,hl=addr)
        call_oz(os_mv)          ;exits with bc=bytes not read
        pop     hl
        and     a
        sbc     hl,bc
#endasm
}
