/*
 * Printf style formatting. (Borrowed from MicroEmacs by Dave Conroy.) 
 * A lot smaller than the full fledged printf.
 *
 * Liberated once more and inserted into Small C+ 29/4/99 djm
 */

#define ANSI_STDIO

#include <stdio.h>


/*
 * Cheating somewhat, this will cause a barf in the compiler...bigtime..
 * nevermind...
 */


int fscanf(FILE *fp, char *fmt,...)
{
        int  *ct;
        ct= (getarg()*2)+&fmt-4;

        return (vfscanf((FILE *)(*ct),(unsigned char *)(*(ct-1)),ct-2));
}

