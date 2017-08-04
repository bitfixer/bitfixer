/*
 *      Part of the library for fcntlt
 *
 *      int open(char *name,int access, mode_t mode)
 *
 *      djm 27/4/99
 *
 *      Access is either
 *
 *      O_RDONLY = 0
 *      O_WRONLY = 1    Starts afresh?!?!?
 *      O_APPEND = 256
 *
 *      All others are ignored(!)
 *
 * -----
 * $Id: open.c,v 1.1 2002/01/27 21:28:48 dom Exp $
 */

#include <stdio.h>
#include <fcntl.h>      /* Or is it unistd.h, who knows! */

int open(far char *name, int flags, mode_t mode)
{
    int fd;
    char	buffer[10];	/* Buffer for expansion */

    return ( open_z88(name,flags,mode,buffer,9) );

}

