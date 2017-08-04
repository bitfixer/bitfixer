;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 3.4.1 #9090 (Nov 13 2014) (Mac OS X x86_64)
; This file was generated Fri Feb 17 01:35:20 2017
;--------------------------------------------------------
	.module dgprog
	.optsdcc -mz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _main
	.globl _enableReset
	.globl _drawToScreen
	.globl _delay
	.globl _readString
	.globl _readchar_blocking
	.globl _readchar
	.globl _print
	.globl _clear
	.globl _printf_small
	.globl _newline
	.globl _printchar
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
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:12: void clear()
;	---------------------------------
; Function clear
; ---------------------------------
_clear_start::
_clear:
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:17: __endasm;
	call #0x00E6
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:21: *lctr = 0;
	ld	hl,#0x0140
	ld	(hl),#0x00
	ret
_clear_end::
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:24: void print(unsigned char* str)
;	---------------------------------
; Function print
; ---------------------------------
_print_start::
_print:
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:48: __endasm;
	    loadstack:
	pop bc
	pop hl
	push hl
	push bc
	ld a, (#0x0140)
	ld b, a
	    printchar:
	ld a,(hl)
	cp a, #0x00
	jp z,done
	set 7, a
	call #0x00FA
	inc hl
	inc b
	jp printchar
	    done:
	ld a, b
	and a, #0x3F ; mod 64
	ld (#0x0140), a ; store line counter value
	ret
_print_end::
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:51: unsigned char readchar()
;	---------------------------------
; Function readchar
; ---------------------------------
_readchar_start::
_readchar:
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:58: __endasm;
	in a, ($00)
	ld (#0x0151), a
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:60: if ((*temp & 0x80) == 0)
	ld	hl,#0x0151
	ld	l,(hl)
	bit	7, l
	ret	NZ
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:62: return 0;
	ld	l,#0x00
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:66: return *temp;
	ret
_readchar_end::
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:69: unsigned char readchar_blocking()
;	---------------------------------
; Function readchar_blocking
; ---------------------------------
_readchar_blocking_start::
_readchar_blocking:
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:73: ch = 0;
	ld	l,#0x00
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:74: while (ch == 0)
00101$:
	ld	a,l
	or	a, a
	jr	NZ,00103$
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:76: ch = readchar();
	call	_readchar
	jr	00101$
00103$:
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:79: ret = ch & 0x7F;
	ld	d,l
	res	7, d
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:81: while (ch != 0)
00104$:
	ld	a,l
	or	a, a
	jr	Z,00106$
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:83: ch = readchar();
	push	de
	call	_readchar
	pop	de
	jr	00104$
00106$:
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:86: return ret;
	ld	l,d
	ret
_readchar_blocking_end::
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:91: void readString(unsigned char* str)
;	---------------------------------
; Function readString
; ---------------------------------
_readString_start::
_readString:
	push	ix
	ld	ix,#0
	add	ix,sp
	push	af
	push	af
	dec	sp
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:95: cc[1] = 0;
	ld	hl,#0x0001
	add	hl,sp
	ld	c,l
	ld	b,h
	inc	hl
	ld	(hl),#0x00
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:96: ch = readchar_blocking();
	push	bc
	call	_readchar_blocking
	pop	bc
	ld	-5 (ix),l
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:97: while (ch != 13)
	ld	-2 (ix),c
	ld	-1 (ix),b
	ld	e,4 (ix)
	ld	d,5 (ix)
00101$:
	ld	a,-5 (ix)
	sub	a, #0x0D
	jr	Z,00103$
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:99: *str = ch;
	ld	a,-5 (ix)
	ld	(de),a
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:100: str++;
	inc	de
	ld	4 (ix),e
	ld	5 (ix),d
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:103: cc[0] = ch;
	ld	a,-5 (ix)
	ld	(bc),a
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:104: print(cc);
	ld	l,-2 (ix)
	ld	h,-1 (ix)
	push	bc
	push	de
	push	hl
	call	_print
	pop	af
	call	_readchar_blocking
	pop	de
	pop	bc
	ld	-5 (ix),l
	jr	00101$
00103$:
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:108: *str = 0;
	xor	a, a
	ld	(de),a
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:109: newline();
	call	_newline
	ld	sp, ix
	pop	ix
	ret
_readString_end::
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:112: void newline()
;	---------------------------------
; Function newline
; ---------------------------------
_newline_start::
_newline:
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:119: sp = 64 - *lctr;
	ld	hl,#0x0140
	ld	h,(hl)
	ld	a,#0x40
	sub	a, h
	ld	d,a
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:120: for (c = 0; c < sp; c++)
	ld	e,#0x00
00103$:
	ld	a,e
	sub	a, d
	jr	NC,00101$
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:122: print(" ");
	ld	hl,#___str_0
	push	de
	push	hl
	call	_print
	pop	af
	pop	de
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:120: for (c = 0; c < sp; c++)
	inc	e
	jr	00103$
00101$:
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:124: *lctr = 0;
	ld	hl,#0x0140
	ld	(hl),#0x00
	ret
_newline_end::
___str_0:
	.ascii " "
	.db 0x00
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:127: void printchar(unsigned char c)
;	---------------------------------
; Function printchar
; ---------------------------------
_printchar_start::
_printchar:
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:141: __endasm;
	ld iy,#2
	add iy,sp
	ld a,0 (iy)
	call #0x00FA
	ld a, (#0x0140)
	inc a
	and a, #0x3F ; mod 64
	ld (#0x0140), a ; store line counter value
	ret
_printchar_end::
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:144: void delay(int count)
;	---------------------------------
; Function delay
; ---------------------------------
_delay_start::
_delay:
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:147: for (i = 0; i < count; i++)
	ld	de,#0x0000
00103$:
	ld	hl,#2
	add	hl,sp
	ld	a,e
	sub	a, (hl)
	ld	a,d
	inc	hl
	sbc	a, (hl)
	jp	PO, 00116$
	xor	a, #0x80
00116$:
	ret	P
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:151: __endasm;
	nop
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:147: for (i = 0; i < count; i++)
	inc	de
	jr	00103$
	ret
_delay_end::
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:155: void drawToScreen(unsigned char* scrbuf)
;	---------------------------------
; Function drawToScreen
; ---------------------------------
_drawToScreen_start::
_drawToScreen:
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:158: print(scrbuf);
	pop	bc
	pop	hl
	push	hl
	push	bc
	push	hl
	call	_print
	pop	af
	ret
_drawToScreen_end::
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:161: void enableReset()
;	---------------------------------
; Function enableReset
; ---------------------------------
_enableReset_start::
_enableReset:
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:166: *marker = 0;
	ld	hl,#0x0100
	ld	(hl),#0x00
	ret
_enableReset_end::
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:241: void main()
;	---------------------------------
; Function main
; ---------------------------------
_main_start::
_main:
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:245: enableReset();
	call	_enableReset
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:247: clear();
	call	_clear
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:251: for (x = 0; x < 10; x++)
	ld	hl,#0x0000
00102$:
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:253: printf_small("Hello! %d", x);
	ld	de,#___str_1
	push	hl
	push	hl
	push	de
	call	_printf_small
	pop	af
	pop	af
	call	_newline
	pop	hl
;/Users/gubbish/bitfixer/bitfixer/dg/dgprogram/dgprog.c:251: for (x = 0; x < 10; x++)
	inc	hl
	ld	a,l
	sub	a, #0x0A
	ld	a,h
	rla
	ccf
	rra
	sbc	a, #0x80
	jr	C,00102$
	ret
_main_end::
___str_1:
	.ascii "Hello! %d"
	.db 0x00
	.area _CODE
	.area _INITIALIZER
	.area _CABS (ABS)
