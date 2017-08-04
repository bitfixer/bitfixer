;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 3.4.1 #9090 (Nov 13 2014) (Mac OS X x86_64)
; This file was generated Fri Feb 17 01:35:19 2017
;--------------------------------------------------------
	.module printf_dg
	.optsdcc -mz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl __ltoa
	.globl _putchar
	.globl _printf_small
;--------------------------------------------------------
; special function registers
;--------------------------------------------------------
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _DATA
_radix:
	.ds 1
_str:
	.ds 2
_val:
	.ds 4
_printf_small_buffer_4_35:
	.ds 12
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _INITIALIZED
_long_flag:
	.ds 1
_string_flag:
	.ds 1
_char_flag:
	.ds 1
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
;/Users/gubbish/sdcc/lib/_printf_small.c:61: void printf_small (char * fmt, ... ) __reentrant
;	---------------------------------
; Function printf_small
; ---------------------------------
_printf_small_start::
_printf_small:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	hl,#-6
	add	hl,sp
	ld	sp,hl
;/Users/gubbish/sdcc/lib/_printf_small.c:65: va_start(ap,fmt);
	ld	hl,#0x000A+1+1
	add	hl,sp
	ex	(sp), hl
00133$:
;/Users/gubbish/sdcc/lib/_printf_small.c:67: for (; *fmt ; fmt++ ) {
	ld	a,4 (ix)
	ld	-2 (ix),a
	ld	a,5 (ix)
	ld	-1 (ix),a
	ld	l,-2 (ix)
	ld	h,-1 (ix)
	ld	e,(hl)
	ld	a,e
	or	a, a
	jp	Z,00134$
;/Users/gubbish/sdcc/lib/_printf_small.c:68: if (*fmt == '%') {
	ld	a,e
	sub	a, #0x25
	jp	NZ,00128$
;/Users/gubbish/sdcc/lib/_printf_small.c:69: long_flag = string_flag = char_flag = 0;
	ld	hl,#_char_flag + 0
	ld	(hl), #0x00
	ld	hl,#_string_flag + 0
	ld	(hl), #0x00
	ld	hl,#_long_flag + 0
	ld	(hl), #0x00
;/Users/gubbish/sdcc/lib/_printf_small.c:70: fmt++ ;
	ld	a,-2 (ix)
	add	a, #0x01
	ld	4 (ix),a
	ld	a,-1 (ix)
	adc	a, #0x00
	ld	5 (ix),a
;/Users/gubbish/sdcc/lib/_printf_small.c:71: switch (*fmt) {
	ld	e,4 (ix)
	ld	d,5 (ix)
	ld	a,(de)
	ld	b,a
;/Users/gubbish/sdcc/lib/_printf_small.c:74: fmt++;
	inc	de
;/Users/gubbish/sdcc/lib/_printf_small.c:71: switch (*fmt) {
	ld	a,b
	cp	a,#0x68
	jr	Z,00102$
	sub	a, #0x6C
	jr	NZ,00103$
;/Users/gubbish/sdcc/lib/_printf_small.c:73: long_flag = 1;
	ld	hl,#_long_flag + 0
	ld	(hl), #0x01
;/Users/gubbish/sdcc/lib/_printf_small.c:74: fmt++;
	ld	4 (ix),e
	ld	5 (ix),d
;/Users/gubbish/sdcc/lib/_printf_small.c:75: break;
	jr	00103$
;/Users/gubbish/sdcc/lib/_printf_small.c:76: case 'h':
00102$:
;/Users/gubbish/sdcc/lib/_printf_small.c:77: char_flag = 1;
	ld	hl,#_char_flag + 0
	ld	(hl), #0x01
;/Users/gubbish/sdcc/lib/_printf_small.c:78: fmt++;
	ld	4 (ix),e
	ld	5 (ix),d
;/Users/gubbish/sdcc/lib/_printf_small.c:79: }
00103$:
;/Users/gubbish/sdcc/lib/_printf_small.c:81: switch (*fmt) {
	ld	l,4 (ix)
	ld	h,5 (ix)
	ld	a,(hl)
	ld	-2 (ix), a
	sub	a, #0x63
	jr	Z,00107$
	ld	a,-2 (ix)
	sub	a, #0x64
	jr	Z,00105$
	ld	a,-2 (ix)
	sub	a, #0x6F
	jr	Z,00108$
	ld	a,-2 (ix)
	sub	a, #0x73
	jr	Z,00104$
	ld	a,-2 (ix)
	sub	a, #0x78
	jr	Z,00106$
	jr	00109$
;/Users/gubbish/sdcc/lib/_printf_small.c:82: case 's':
00104$:
;/Users/gubbish/sdcc/lib/_printf_small.c:83: string_flag = 1;
	ld	hl,#_string_flag + 0
	ld	(hl), #0x01
;/Users/gubbish/sdcc/lib/_printf_small.c:84: break;
	jr	00109$
;/Users/gubbish/sdcc/lib/_printf_small.c:85: case 'd':
00105$:
;/Users/gubbish/sdcc/lib/_printf_small.c:86: radix = 10;
	ld	hl,#_radix + 0
	ld	(hl), #0x0A
;/Users/gubbish/sdcc/lib/_printf_small.c:87: break;
	jr	00109$
;/Users/gubbish/sdcc/lib/_printf_small.c:88: case 'x':
00106$:
;/Users/gubbish/sdcc/lib/_printf_small.c:89: radix = 16;
	ld	hl,#_radix + 0
	ld	(hl), #0x10
;/Users/gubbish/sdcc/lib/_printf_small.c:90: break;
	jr	00109$
;/Users/gubbish/sdcc/lib/_printf_small.c:91: case 'c':
00107$:
;/Users/gubbish/sdcc/lib/_printf_small.c:92: radix = 0;
	ld	hl,#_radix + 0
	ld	(hl), #0x00
;/Users/gubbish/sdcc/lib/_printf_small.c:93: break;
	jr	00109$
;/Users/gubbish/sdcc/lib/_printf_small.c:94: case 'o':
00108$:
;/Users/gubbish/sdcc/lib/_printf_small.c:95: radix = 8;
	ld	hl,#_radix + 0
	ld	(hl), #0x08
;/Users/gubbish/sdcc/lib/_printf_small.c:97: }
00109$:
;/Users/gubbish/sdcc/lib/_printf_small.c:101: str = va_arg(ap, char *);
	pop	de
	push	de
	inc	de
	inc	de
	ld	a,e
	add	a,#0xFE
	ld	-2 (ix),a
	ld	a,d
	adc	a,#0xFF
	ld	-1 (ix),a
;/Users/gubbish/sdcc/lib/_printf_small.c:99: if (string_flag)
	ld	a,(#_string_flag + 0)
	or	a, a
	jr	Z,00114$
;/Users/gubbish/sdcc/lib/_printf_small.c:101: str = va_arg(ap, char *);
	inc	sp
	inc	sp
	push	de
	ld	l,-2 (ix)
	ld	h,-1 (ix)
	ld	a,(hl)
	ld	iy,#_str
	ld	0 (iy),a
	inc	hl
	ld	a,(hl)
	ld	(#_str + 1),a
;/Users/gubbish/sdcc/lib/_printf_small.c:102: while (*str) putchar(*str++);
00110$:
	ld	hl,(_str)
	ld	d,(hl)
	ld	a,d
	or	a, a
	jp	Z,00130$
	ld	hl, #_str+0
	inc	(hl)
	jr	NZ,00219$
	ld	hl, #_str+1
	inc	(hl)
00219$:
	push	de
	inc	sp
	call	_putchar
	inc	sp
	jr	00110$
;/Users/gubbish/sdcc/lib/_printf_small.c:103: continue ;
00114$:
;/Users/gubbish/sdcc/lib/_printf_small.c:106: if (long_flag)
	ld	a,(#_long_flag + 0)
	or	a, a
	jr	Z,00119$
;/Users/gubbish/sdcc/lib/_printf_small.c:107: val = va_arg(ap,long);
	ld	a,-6 (ix)
	add	a, #0x04
	ld	l,a
	ld	a,-5 (ix)
	adc	a, #0x00
	ld	h,a
	inc	sp
	inc	sp
	push	hl
	ld	a,l
	add	a,#0xFC
	ld	l,a
	ld	a,h
	adc	a,#0xFF
	ld	-4 (ix), l
	ld	-3 (ix), a
	ld	l,-4 (ix)
	ld	h,-3 (ix)
	ld	a,(hl)
	ld	iy,#_val
	ld	0 (iy),a
	inc	hl
	ld	a,(hl)
	ld	iy,#_val
	ld	1 (iy),a
	inc	hl
	ld	a,(hl)
	ld	iy,#_val
	ld	2 (iy),a
	inc	hl
	ld	a,(hl)
	ld	iy,#_val
	ld	3 (iy),a
	jr	00120$
00119$:
;/Users/gubbish/sdcc/lib/_printf_small.c:109: if (char_flag)
	ld	a,(#_char_flag + 0)
	or	a, a
	jr	Z,00116$
;/Users/gubbish/sdcc/lib/_printf_small.c:110: val = va_arg(ap,char);
	pop	hl
	push	hl
	inc	hl
	inc	sp
	inc	sp
	push	hl
	dec	hl
	ld	a,(hl)
	ld	iy,#_val
	ld	0 (iy),a
	rla
	sbc	a, a
	ld	iy,#_val
	ld	1 (iy),a
	ld	iy,#_val
	ld	2 (iy),a
	ld	iy,#_val
	ld	3 (iy),a
	jr	00120$
00116$:
;/Users/gubbish/sdcc/lib/_printf_small.c:112: val = va_arg(ap,int);
	inc	sp
	inc	sp
	push	de
	ld	l,-2 (ix)
	ld	h,-1 (ix)
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	ld	iy,#_val
	ld	0 (iy),e
	ld	iy,#_val
	ld	1 (iy),d
	ld	a,d
	rla
	sbc	a, a
	ld	iy,#_val
	ld	2 (iy),a
	ld	iy,#_val
	ld	3 (iy),a
00120$:
;/Users/gubbish/sdcc/lib/_printf_small.c:114: if (radix)
	ld	a,(#_radix + 0)
	or	a, a
	jr	Z,00125$
;/Users/gubbish/sdcc/lib/_printf_small.c:119: _ltoa (val, buffer, radix);
	ld	de,#_printf_small_buffer_4_35
	ld	a,(_radix)
	push	af
	inc	sp
	push	de
	ld	hl,(_val + 2)
	push	hl
	ld	hl,(_val)
	push	hl
	call	__ltoa
	ld	hl,#7
	add	hl,sp
	ld	sp,hl
;/Users/gubbish/sdcc/lib/_printf_small.c:120: stri = buffer;
	ld	hl,#_printf_small_buffer_4_35
;/Users/gubbish/sdcc/lib/_printf_small.c:121: while (*stri)
00121$:
	ld	a,(hl)
	or	a, a
	jr	Z,00130$
;/Users/gubbish/sdcc/lib/_printf_small.c:123: putchar (*stri);
	push	hl
	push	af
	inc	sp
	call	_putchar
	inc	sp
	pop	hl
;/Users/gubbish/sdcc/lib/_printf_small.c:124: stri++;
	inc	hl
	jr	00121$
00125$:
;/Users/gubbish/sdcc/lib/_printf_small.c:128: putchar((char)val);
	ld	iy,#_val
	ld	h,0 (iy)
	push	hl
	inc	sp
	call	_putchar
	inc	sp
	jr	00130$
00128$:
;/Users/gubbish/sdcc/lib/_printf_small.c:131: putchar(*fmt);
	ld	a,e
	push	af
	inc	sp
	call	_putchar
	inc	sp
00130$:
;/Users/gubbish/sdcc/lib/_printf_small.c:67: for (; *fmt ; fmt++ ) {
	inc	4 (ix)
	jp	NZ,00133$
	inc	5 (ix)
	jp	00133$
00134$:
	ld	sp, ix
	pop	ix
	ret
_printf_small_end::
	.area _CODE
	.area _INITIALIZER
__xinit__long_flag:
	.db #0x00	; 0
__xinit__string_flag:
	.db #0x00	; 0
__xinit__char_flag:
	.db #0x00	; 0
	.area _CABS (ABS)
