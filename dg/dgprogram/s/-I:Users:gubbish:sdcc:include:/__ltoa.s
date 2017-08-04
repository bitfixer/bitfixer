;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 3.4.1 #9090 (Nov 13 2014) (Mac OS X x86_64)
; This file was generated Fri Feb 17 01:35:19 2017
;--------------------------------------------------------
	.module __ltoa
	.optsdcc -mz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl __ltoa
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
;/Users/gubbish/sdcc/lib/__ltoa.c:48: void _ltoa(long value, char* string, unsigned char radix)
;	---------------------------------
; Function _ltoa
; ---------------------------------
__ltoa_start::
__ltoa:
	push	ix
	ld	ix,#0
	add	ix,sp
;/Users/gubbish/sdcc/lib/__ltoa.c:50: if (value < 0 && radix == 10) 
	bit	7, 7 (ix)
	jr	Z,00102$
	ld	a,10 (ix)
	sub	a, #0x0A
	jr	NZ,00102$
;/Users/gubbish/sdcc/lib/__ltoa.c:52: *string++ = '-';
	ld	l,8 (ix)
	ld	h,9 (ix)
	ld	(hl),#0x2D
	inc	hl
	ld	8 (ix),l
	ld	9 (ix),h
;/Users/gubbish/sdcc/lib/__ltoa.c:53: value = -value;
	xor	a, a
	sub	a, 4 (ix)
	ld	4 (ix),a
	ld	a, #0x00
	sbc	a, 5 (ix)
	ld	5 (ix),a
	ld	a, #0x00
	sbc	a, 6 (ix)
	ld	6 (ix),a
	ld	a, #0x00
	sbc	a, 7 (ix)
	ld	7 (ix),a
00102$:
;/Users/gubbish/sdcc/lib/__ltoa.c:55: _ultoa(value, string, radix);
	ld	a,10 (ix)
	push	af
	inc	sp
	ld	l,8 (ix)
	ld	h,9 (ix)
	push	hl
	ld	l,6 (ix)
	ld	h,7 (ix)
	push	hl
	ld	l,4 (ix)
	ld	h,5 (ix)
	push	hl
	call	__ultoa
	ld	hl,#7
	add	hl,sp
	ld	sp,hl
	pop	ix
	ret
__ltoa_end::
	.area _CODE
	.area _INITIALIZER
	.area _CABS (ABS)
