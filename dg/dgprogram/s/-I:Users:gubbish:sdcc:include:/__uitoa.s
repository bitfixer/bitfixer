;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 3.4.1 #9090 (Nov 13 2014) (Mac OS X x86_64)
; This file was generated Fri Feb 17 01:35:20 2017
;--------------------------------------------------------
	.module __uitoa
	.optsdcc -mz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl __uitoa
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
;/Users/gubbish/sdcc/lib/__uitoa.c:37: void _uitoa(unsigned int value, char* string, unsigned char radix)
;	---------------------------------
; Function _uitoa
; ---------------------------------
__uitoa_start::
__uitoa:
	push	ix
	ld	ix,#0
	add	ix,sp
	push	af
	dec	sp
;/Users/gubbish/sdcc/lib/__uitoa.c:42: do 
	ld	c,#0x00
00103$:
;/Users/gubbish/sdcc/lib/__uitoa.c:44: string[index] = '0' + (value % radix);
	ld	l,6 (ix)
	ld	h,7 (ix)
	ld	b,#0x00
	add	hl, bc
	ld	e,8 (ix)
	ld	d,#0x00
	push	hl
	push	bc
	push	de
	push	de
	ld	l,4 (ix)
	ld	h,5 (ix)
	push	hl
	call	__moduint
	pop	af
	pop	af
	ld	-1 (ix),h
	ld	-2 (ix),l
	pop	de
	pop	bc
	pop	hl
	ld	a,-2 (ix)
	add	a, #0x30
	ld	b,a
	ld	(hl),b
;/Users/gubbish/sdcc/lib/__uitoa.c:45: if (string[index] > '9')
	ld	a,#0x39
	sub	a, b
	jp	PO, 00133$
	xor	a, #0x80
00133$:
	jp	P,00102$
;/Users/gubbish/sdcc/lib/__uitoa.c:46: string[index] += 'A' - '9' - 1;
	ld	a,(hl)
	add	a, #0x07
	ld	(hl),a
00102$:
;/Users/gubbish/sdcc/lib/__uitoa.c:47: value /= radix;
	push	bc
	push	de
	ld	l,4 (ix)
	ld	h,5 (ix)
	push	hl
	call	__divuint
	pop	af
	pop	af
	pop	bc
	ld	4 (ix),l
	ld	5 (ix),h
;/Users/gubbish/sdcc/lib/__uitoa.c:48: ++index;
	inc	c
;/Users/gubbish/sdcc/lib/__uitoa.c:50: while (value != 0);
	ld	a,5 (ix)
	or	a,4 (ix)
	jr	NZ,00103$
;/Users/gubbish/sdcc/lib/__uitoa.c:53: string[index--] = '\0';
	ld	d,c
	dec	d
	ld	l,6 (ix)
	ld	h,7 (ix)
	ld	b,#0x00
	add	hl, bc
	ld	(hl),#0x00
;/Users/gubbish/sdcc/lib/__uitoa.c:56: while (index > i) 
	ld	e,#0x00
00106$:
	ld	a,e
	sub	a, d
	jp	PO, 00134$
	xor	a, #0x80
00134$:
	jp	P,00109$
;/Users/gubbish/sdcc/lib/__uitoa.c:58: char tmp = string[i];
	ld	a,6 (ix)
	add	a, e
	ld	c,a
	ld	a,7 (ix)
	adc	a, #0x00
	ld	b,a
	ld	a,(bc)
	ld	-3 (ix),a
;/Users/gubbish/sdcc/lib/__uitoa.c:59: string[i] = string[index];
	ld	a,6 (ix)
	add	a, d
	ld	-2 (ix),a
	ld	a,7 (ix)
	adc	a, #0x00
	ld	-1 (ix),a
	ld	l,-2 (ix)
	ld	h,-1 (ix)
	ld	a,(hl)
	ld	(bc),a
;/Users/gubbish/sdcc/lib/__uitoa.c:60: string[index] = tmp;
	ld	l,-2 (ix)
	ld	h,-1 (ix)
	ld	a,-3 (ix)
	ld	(hl),a
;/Users/gubbish/sdcc/lib/__uitoa.c:61: ++i;
	inc	e
;/Users/gubbish/sdcc/lib/__uitoa.c:62: --index;
	dec	d
	jr	00106$
00109$:
	ld	sp, ix
	pop	ix
	ret
__uitoa_end::
	.area _CODE
	.area _INITIALIZER
	.area _CABS (ABS)
