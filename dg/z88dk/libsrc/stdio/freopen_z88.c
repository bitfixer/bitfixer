/*
 *	Open a file (stdio library) returning the explicit
 *	filename (this reuses a filebuffer)
 *
 *	Only hands r,w,a types (no modifiers)
 *
 *	djm 24/3/2000
 *
 *	djm 1/4/2000 Modified to enable file structures
 *
 * --------
 * $Id: freopen_z88.c,v 1.8 2012/06/13 11:04:30 stefano Exp $
 */

#define ANSI_STDIO
#include <stdio.h>
#include <fcntl.h>




FILE *freopen_z88(far char *name, unsigned char *mode, FILE *fp, char *explicit, size_t len)
{
	int	access;
	int	flags;
	int fd;
	FILE *fp2;
	
	switch (*(unsigned char *)mode) {
		case 'r':
			access=O_RDONLY;
			flags=_IOREAD | _IOUSE| _IOTEXT;
			break;
		case 'w':
			access=O_WRONLY;
                	flags = _IOWRITE | _IOUSE | _IOTEXT;
			break;
		case 'a':
			access=O_APPEND;
                	flags = _IOWRITE | _IOUSE | _IOTEXT;
			break;
		default:
			return (FILE *)NULL;
	}
#ifdef __STDIO_BINARY
	if ( *(unsigned char *) (mode+1) == 'b' )
	    flags ^= _IOTEXT;
#endif

#ifdef NET_STDIO
	if (opennet(fp,name,explicit,len) ) return (fp);
#endif
	{

		// Others, pass int mode
		fd=open_z88(name,access,mode,explicit,len);

		fp2=fp;
		if (fd == - 1 ) return (FILE *)NULL;
        	fp2->desc.fd=fd;
        	fp2->ungetc=0; 
        	fp2->flags=flags;
		return fp2;
	}
}
