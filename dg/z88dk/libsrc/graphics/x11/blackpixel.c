/*
	Minimal Xlib port
	Stefano Bodrato, 5/3/2007
	
	$Id: blackpixel.c,v 1.1 2007/12/21 08:04:23 stefano Exp $
*/

#define _BUILDING_X
#include <X11/Xlib.h>


int BlackPixel(Display *display, int screen) {
	return 1;
}
