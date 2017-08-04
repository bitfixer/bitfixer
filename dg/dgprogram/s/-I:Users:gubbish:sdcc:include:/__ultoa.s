;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 3.4.1 #9090 (Nov 13 2014) (Mac OS X x86_64)
; This file was generated Fri Feb 17 01:35:19 2017
;--------------------------------------------------------
	.module __ultoa
	.optsdcc -mz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl __ultoa
;--------------------------------------------------------
; special function registers
;--------------------------------------------------------
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _DATA
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _INITIALIZED
;--------------------------------------------------------
; absolute external ram data
;--------------------------------------------------------
	.area _DABS (ABS)
;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
	.area _HOME
	.area _GSINIT
	.area _GSFINAL
	.area _GSINIT
;--------------------------------------------------------
; Home
;--------------------------------------------------------
	.area _HOME
	.area _HOME
;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area _CODE
;/Users/gubbish/sdcc/lib/__ultoa.c:48: void _ultoa(unsigned long value, char* string, unsigned char radix)
;	---------------------------------
; Function _ultoa
; ---------------------------------
__ultoa_start::
__ultoa:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	hl,#-37
	add	hl,sp
	ld	sp,hl
;/Users/gubbish/sdcc/lib/__ultoa.c:53: do 
	ld	hl,#0x0001
	add	hl,sp
	ld	c,l
	ld	b,h
	ld	-37 (ix),#0x20
00103$:
;/Users/gubbish/sdcc/lib/__ultoa.c:55: unsigned char c = '0' + (value % radix);
	ld	a,10 (ix)
	ld	-4 (ix),a
	ld	-3 (ix),#0x00
	ld	-2 (ix),#0x00
	ld	-1 (ix),#0x00
	push	bc
	ld	l,-2 (ix)
	ld	h,-1 (ix)
	push	hl
	ld	l,-4 (ix)
	ld	h,-3 (ix)
	push	hl
	ld	l,6 (ix)
	ld	h,7 (ix)
	push	hl
	ld	l,4 (ix)
	ld	h,5 (ix)
	push	hl
	call	__modulong
	pop	af
	pop	af
	pop	af
	pop	af
	pop	bc
	ld	a,l
	add	a, #0x30
	ld	d,a
;/Users/gubbish/sdcc/lib/__ultoa.c:56: if (c > (unsigned char)'9')
	ld	a,#0x39
	sub	a, d
	jr	NC,00102$
;/Users/gubbish/sdcc/lib/__ultoa.c:57: c += 'A' - '9' - 1;
	ld	a,d
	add	a, #0x07
	ld	d,a
00102$:
;/Users/gubbish/sdcc/lib/__ultoa.c:58: buffer[--index] = c;
	dec	-37 (ix)
	ld	l,-37 (ix)
	ld	h,#0x00
	add	hl,bc
	ld	(hl),d
;/Users/gubbish/sdcc/lib/__ultoa.c:59: value /= radix;
	push	bc
	ld	l,-2 (ix)
	ld	h,-1 (ix)
	push	hl
	ld	l,-4 (ix)
	ld	h,-3 (ix)
	push	hl
	ld	l,6 (ix)
	ld	h,7 (ix)
	push	hl
	ld	l,4 (ix)
	ld	h,5 (ix)
	push	hl
	call	__divulong
	pop	af
	pop	af
	pop	af
	pop	af
	pop	bc
	ld	4 (ix),l
	ld	5 (ix),h
	ld	6 (ix),e
;/Users/gubbish/sdcc/lib/__ultoa.c:61: while (value);
	ld	7 (ix), d
	ld	a, d
	or	a, 6 (ix)
	or	a, 5 (ix)
	or	a,4 (ix)
	jp	NZ,00103$
;/Users/gubbish/sdcc/lib/__ultoa.c:63: do 
	ld	e,8 (ix)
	ld	d,9 (ix)
	push	de
	pop	iy
	ld	d,-37 (ix)
00106$:
;/Users/gubbish/sdcc/lib/__ultoa.c:65: *string++ = buffer[index];
	ld	l,d
	ld	h,#0x00
	add	hl,bc
	ld	a,(hl)
	ld	0 (iy), a
	inc	iy
;/Users/gubbish/sdcc/lib/__ultoa.c:67: while ( ++index != NUMBER_OF_DIGITS );
	inc	d
	ld	a,d
	sub	a, #0x20
	jr	NZ,00106$
;/Users/gubbish/sdcc/lib/__ultoa.c:69: *string = 0;  /* string terminator */
	ld	0 (iy), #0x00
	ld	sp, ix
	pop	ix
	ret
__ultoa_end::
	.area _CODE
	.area _INITIALIZER
	.area _CABS (ABS)
