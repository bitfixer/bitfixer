;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 3.4.1 #9090 (Nov 13 2014) (Mac OS X x86_64)
; This file was generated Fri Feb 17 01:35:20 2017
;--------------------------------------------------------
	.module _malloc
	.optsdcc -mz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _malloc
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
;/Users/gubbish/sdcc/lib/_malloc.c:60: void * malloc (unsigned int size)
;	---------------------------------
; Function malloc
; ---------------------------------
_malloc_start::
_malloc:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	hl,#-10
	add	hl,sp
	ld	sp,hl
;/Users/gubbish/sdcc/lib/_malloc.c:66: if (size>(0xFFFF-HEADER_SIZE))
	ld	a,#0xF9
	cp	a, 4 (ix)
	ld	a,#0xFF
	sbc	a, 5 (ix)
	jr	NC,00102$
;/Users/gubbish/sdcc/lib/_malloc.c:68: return NULL; //To prevent overflow in next line
	ld	hl,#0x0000
	jp	00117$
00102$:
;/Users/gubbish/sdcc/lib/_malloc.c:71: size += HEADER_SIZE; //We need a memory for header too
	ld	a,4 (ix)
	add	a, #0x06
	ld	4 (ix),a
	ld	a,5 (ix)
	adc	a, #0x00
	ld	5 (ix),a
;/Users/gubbish/sdcc/lib/_malloc.c:72: current_header = &_sdcc_heap_start;
	ld	-8 (ix),#<(__sdcc_heap_start)
	ld	-7 (ix),#>(__sdcc_heap_start)
;/Users/gubbish/sdcc/lib/_malloc.c:121: }
	ld	a,i
	di
	push	af
;/Users/gubbish/sdcc/lib/_malloc.c:76: while (1)
00108$:
;/Users/gubbish/sdcc/lib/_malloc.c:85: if ((((unsigned int)current_header->next) -
	ld	l,-8 (ix)
	ld	h,-7 (ix)
	ld	a,(hl)
	ld	-2 (ix),a
	inc	hl
	ld	a,(hl)
	ld	-1 (ix),a
	ld	a,-2 (ix)
	ld	-4 (ix),a
	ld	a,-1 (ix)
	ld	-3 (ix),a
;/Users/gubbish/sdcc/lib/_malloc.c:86: ((unsigned int)current_header) -
	ld	a,-8 (ix)
	ld	-6 (ix),a
	ld	a,-7 (ix)
	ld	-5 (ix),a
	ld	a,-4 (ix)
	sub	a, -6 (ix)
	ld	-6 (ix),a
	ld	a,-3 (ix)
	sbc	a, -5 (ix)
	ld	-5 (ix),a
;/Users/gubbish/sdcc/lib/_malloc.c:87: current_header->len) >= size)
	ld	a,-8 (ix)
	ld	-4 (ix),a
	ld	a,-7 (ix)
	ld	-3 (ix),a
	ld	l,-4 (ix)
	ld	h,-3 (ix)
	ld	de, #0x0004
	add	hl, de
	ld	a,(hl)
	ld	-4 (ix),a
	inc	hl
	ld	a,(hl)
	ld	-3 (ix),a
	ld	a,-6 (ix)
	sub	a, -4 (ix)
	ld	-6 (ix),a
	ld	a,-5 (ix)
	sbc	a, -3 (ix)
	ld	-5 (ix),a
	ld	a,-6 (ix)
	sub	a, 4 (ix)
	ld	a,-5 (ix)
	sbc	a, 5 (ix)
	jr	C,00104$
;/Users/gubbish/sdcc/lib/_malloc.c:89: ret = &current_header->mem;
	ld	a,-8 (ix)
	add	a, #0x06
	ld	-6 (ix),a
	ld	a,-7 (ix)
	adc	a, #0x00
	ld	-5 (ix),a
	ld	a,-6 (ix)
	ld	-10 (ix),a
	ld	a,-5 (ix)
	ld	-9 (ix),a
;/Users/gubbish/sdcc/lib/_malloc.c:90: break;
	jr	00109$
00104$:
;/Users/gubbish/sdcc/lib/_malloc.c:92: current_header = current_header->next;    //else try next
	ld	h,-2 (ix)
	ld	l,-1 (ix)
	ld	-8 (ix),h
	ld	-7 (ix),l
;/Users/gubbish/sdcc/lib/_malloc.c:85: if ((((unsigned int)current_header->next) -
	ld	l,-8 (ix)
	ld	h,-7 (ix)
	ld	a,(hl)
	ld	-2 (ix),a
	inc	hl
	ld	a,(hl)
;/Users/gubbish/sdcc/lib/_malloc.c:93: if (!current_header->next)
	ld	-1 (ix), a
	or	a,-2 (ix)
	jp	NZ,00108$
;/Users/gubbish/sdcc/lib/_malloc.c:95: ret = NULL;
	ld	-10 (ix),#0x00
	ld	-9 (ix),#0x00
;/Users/gubbish/sdcc/lib/_malloc.c:96: break;
00109$:
;/Users/gubbish/sdcc/lib/_malloc.c:100: if (ret)
	ld	a,-9 (ix)
	or	a,-10 (ix)
	jr	Z,00116$
;/Users/gubbish/sdcc/lib/_malloc.c:102: if (!current_header->len)
	ld	a,-8 (ix)
	add	a, #0x04
	ld	l,a
	ld	a,-7 (ix)
	adc	a, #0x00
	ld	h,a
	push	hl
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	pop	hl
	ld	a,d
	or	a,e
	jr	NZ,00113$
;/Users/gubbish/sdcc/lib/_malloc.c:104: current_header->len = size; //for first allocation
	ld	a,4 (ix)
	ld	(hl),a
	inc	hl
	ld	a,5 (ix)
	ld	(hl),a
	jr	00116$
00113$:
;/Users/gubbish/sdcc/lib/_malloc.c:109: new_header = (MEMHEADER * )((char *)current_header + current_header->len);
	ld	l,-8 (ix)
	ld	h,-7 (ix)
	add	hl,de
;/Users/gubbish/sdcc/lib/_malloc.c:110: new_header->next = current_header->next; //and plug it into the chain
	ld	c,l
	ld	b,h
	ld	a,-2 (ix)
	ld	(hl),a
	inc	hl
	ld	a,-1 (ix)
	ld	(hl),a
;/Users/gubbish/sdcc/lib/_malloc.c:111: new_header->prev = current_header;
	ld	l, c
	ld	h, b
	inc	hl
	inc	hl
	ld	a,-8 (ix)
	ld	(hl),a
	inc	hl
	ld	a,-7 (ix)
	ld	(hl),a
;/Users/gubbish/sdcc/lib/_malloc.c:112: current_header->next  = new_header;
	ld	l,-8 (ix)
	ld	h,-7 (ix)
	ld	(hl),c
	inc	hl
	ld	(hl),b
;/Users/gubbish/sdcc/lib/_malloc.c:113: if (new_header->next)
	ld	l, c
	ld	h, b
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	ld	a,-1 (ix)
	or	a,-2 (ix)
	jr	Z,00111$
;/Users/gubbish/sdcc/lib/_malloc.c:115: new_header->next->prev = new_header;
	ex	de,hl
	inc	hl
	inc	hl
	ld	(hl),c
	inc	hl
	ld	(hl),b
00111$:
;/Users/gubbish/sdcc/lib/_malloc.c:117: new_header->len  = size; //mark as used
	ld	hl,#0x0004
	add	hl,bc
	ld	a,4 (ix)
	ld	(hl),a
	inc	hl
	ld	a,5 (ix)
	ld	(hl),a
;/Users/gubbish/sdcc/lib/_malloc.c:118: ret = &new_header->mem;
	ld	hl,#0x0006
	add	hl,bc
	ld	-10 (ix),l
	ld	-9 (ix),h
00116$:
	pop	af
	jp	PO,00148$
	ei
00148$:
;/Users/gubbish/sdcc/lib/_malloc.c:122: return ret;
	pop	hl
	push	hl
00117$:
	ld	sp, ix
	pop	ix
	ret
_malloc_end::
;/Users/gubbish/sdcc/lib/_malloc.c:146: static void z(void) __naked
;	---------------------------------
; Function z
; ---------------------------------
_z:
;/Users/gubbish/sdcc/lib/_malloc.c:148: __asm__(".area _GSINIT\n");		/* #pragma GSINIT can't be used because it works on the whole file :-( */
	.area _GSINIT
;/Users/gubbish/sdcc/lib/_malloc.c:151: static void sdcc_heap_init(void) __naked
;	---------------------------------
; Function sdcc_heap_init
; ---------------------------------
_sdcc_heap_init:
;/Users/gubbish/sdcc/lib/_malloc.c:153: MEMHEADER * pbase = &_sdcc_heap_start;
;/Users/gubbish/sdcc/lib/_malloc.c:154: unsigned int size = &_sdcc_heap_end - (char*)pbase;
	ld	hl,#__sdcc_heap_end+0
	ld	de,#__sdcc_heap_start
	cp	a, a
	sbc	hl, de
;/Users/gubbish/sdcc/lib/_malloc.c:156: pbase->next = (MEMHEADER*) ((char*)pbase + size - HEADER_SIZE);
	add	hl,de
	ld	a,l
	add	a,#0xFA
	ld	e,a
	ld	a,h
	adc	a,#0xFF
	ld	d,a
	ld	(__sdcc_heap_start), de
;/Users/gubbish/sdcc/lib/_malloc.c:157: pbase->next->next = NULL; 	/* And mark it as last		*/
	ld	a,#0x00
	ld	(de),a
	inc	de
	ld	a,#0x00
	ld	(de),a
;/Users/gubbish/sdcc/lib/_malloc.c:158: pbase->prev       = NULL; 	/* and mark first as first	*/
	ld	hl,#0x0000
	ld	((__sdcc_heap_start + 0x0002)), hl
;/Users/gubbish/sdcc/lib/_malloc.c:159: pbase->len        = 0;    	/* Empty and ready.			*/
	ld	l, #0x00
	ld	((__sdcc_heap_start + 0x0004)), hl
	.area _CODE
	.area _INITIALIZER
	.area _CABS (ABS)
