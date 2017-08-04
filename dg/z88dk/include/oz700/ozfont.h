/* 
	HTC Compatibility Library and OZ extras 
	Fonts

	$Id: ozfont.h,v 1.4 2003/10/23 10:42:49 stefano Exp $
*/

#ifndef _OZFONT_H
#define _OZFONT_H


#ifndef _OZ_BYTE
typedef unsigned char byte;
#define _OZ_BYTE
#endif


/* 12 bytes long */
struct ozfontheader
{
    unsigned checksum;
    unsigned magic; /* 0x466F */
    unsigned length; /* including length of header */
    byte first;
    byte last;
    byte bitmap_height;
    byte line_height; /* recommended */
    byte maxwidth;
    byte active;
};

struct ozfonttableentry
{
    byte width;
    unsigned offset;
};

extern byte *ozfontpointers[];
extern byte ozcustomactivefont;

extern byte _LIB_ ozscancustomfonts(void);
extern void _LIB_ ozsetfontpointer(byte *font);
extern void _LIB_ ozclearcustomfontslot(byte slot);
extern void _LIB_ ozwritecustomfontbyte(byte slot,unsigned offset,byte value);
extern void _LIB_ ozwritecustomfontmem(byte slot,byte *fontdata);

#define MASK_CUSTOM0 1
#define MASK_CUSTOM1 2
#define FONT_CUSTOM0 4
#define FONT_CUSTOM1 5
#define FONT_TEMPORARY 6
#define MAX_CUSTOM_SLOT_SIZE 0x0D80


#endif
