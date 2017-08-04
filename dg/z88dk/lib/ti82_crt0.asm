;	Stub for the TI 82 calculator
;
;	Stefano Bodrato - Dec 2000
;
;	$Id: ti82_crt0.asm,v 1.24 2015/01/21 07:05:00 stefano Exp $
;
;-----------------------------------------------------
; Some general PUBLICs and EXTERNs needed by the assembler
;-----------------------------------------------------

	MODULE  Ti82_crt0

	EXTERN	_main		; No matter what set up we have, main is
				;  always, always external to this file.

	PUBLIC	cleanup		; used by exit()
	PUBLIC	l_dcal		; used by calculated calls  = "call (hl)"

	PUBLIC	_vfprintf	; vprintf is internal to this file so we
				;  only ever include one of the set of
				;  routines

	PUBLIC	exitsp		; Exit variables
	PUBLIC	exitcount	;

	PUBLIC	heaplast	;Near malloc heap variables
	PUBLIC	heapblocks

	PUBLIC	__sgoioblk	; For stdin, stdout, stder

	PUBLIC	base_graphics	; Graphics stuff
	PUBLIC	coords		;

	PUBLIC	snd_tick	; Sound variable
	PUBLIC	bit_irqstatus	; current irq status when DI is necessary

	PUBLIC	cpygraph	; TI calc specific stuff
	PUBLIC	tidi		;
	PUBLIC	tiei		;

;-------------------------
; Begin of (shell) headers
;-------------------------

	INCLUDE "Ti82.def"	; ROM / RAM adresses on Ti82
	INCLUDE	"zcc_opt.def"	; Receive all compiler-defines
	
;OS82Head:
;    defb     $FE,$82,$0F
;
;AshHead:
;    defb     $D9,$00,$20
;
;CrASHhead:
;    defb     $D5,$00,$11


;-------------------
;1 - CrASH (default)
;-------------------
	org	START_ADDR-3
	DEFB $D5,$00,$11
;	org	START_ADDR
	DEFINE NEED_name
	INCLUDE	"zcc_opt.def"
	UNDEFINE NEED_name
IF !DEFINED_NEED_name
	defm	"Z88DK Small C+ Program"
ENDIF
	defb	$0		; Termination zero

;-------------------------------------
; End of header, begin of startup part
;-------------------------------------
start:
	ld	hl,0
	add	hl,sp
	ld	(start1+1),hl
IF !DEFINED_atexit		; Less stack use
	ld	hl,-6		; 3 pointers (more likely value)
	add	hl,sp
	ld	sp,hl
	ld	(exitsp),sp
ELSE
	ld	hl,-64		; 32 pointers (ANSI standard)
	add	hl,sp
	ld	sp,hl
	ld	(exitsp),sp
ENDIF

; Optional definition for auto MALLOC init
; it assumes we have free space between the end of 
; the compiled program and the stack pointer
	IF DEFINED_USING_amalloc
		INCLUDE "amalloc.def"
	ENDIF

	EXTERN	fputc_cons
	ld	hl,12
	push	hl
	call	fputc_cons
	pop	hl


IF DEFINED_GRAYlib
	INCLUDE	"gray82.asm"
ELSE
	INCLUDE "intwrap82.asm"
ENDIF

	im	2
	call	_main
cleanup:			; exit() jumps to this point
start1:	ld	sp,0		; writeback
	ld	iy,_IY_TABLE	; Restore flag-pointer
	im	1
tiei:	ei
IF DEFINED_GRAYlib
cpygraph:			; little opt :)
ENDIF
tidi:	ret

;----------------------------------------
; End of startup part, routines following
;----------------------------------------
l_dcal:
	jp	(hl)


;-----------
; Define the stdin/out/err area. For the z88 we have two models - the
; classic (kludgey) one and "ANSI" model
;-----------
__sgoioblk:
IF DEFINED_ANSIstdio
	INCLUDE	"stdio_fp.asm"
ELSE
        defw    -11,-12,-10
ENDIF


;---------------------------------
; Select which printf core we want
;---------------------------------
_vfprintf:
IF DEFINED_floatstdio
        EXTERN     vfprintf_fp
        jp      vfprintf_fp
ELSE
        IF DEFINED_complexstdio
                EXTERN     vfprintf_comp
                jp      vfprintf_comp
        ELSE
                IF DEFINED_ministdio
                        EXTERN     vfprintf_mini
                        jp      vfprintf_mini
                ENDIF
        ENDIF
ENDIF


;Seed for integer rand() routines
IF !DEFINED_HAVESEED
                PUBLIC    _std_seed        ;Integer rand() seed
_std_seed:      defw    0       ; Seed for integer rand() routines
ENDIF

;Atexit routine
exitsp:		defw	0
exitcount:	defb	0

;Heap stuff (already needed?)
heaplast:	defw	0
heapblocks:	defw	0

IF DEFINED_USING_amalloc
EXTERN ASMTAIL
PUBLIC _heap
; The heap pointer will be wiped at startup,
; but first its value (based on ASMTAIL)
; will be kept for sbrk() to setup the malloc area
_heap:
                defw ASMTAIL	; Location of the last program byte
                defw 0
ENDIF

;mem stuff
base_graphics:	defw	GRAPH_MEM
coords:		defw	0

IF DEFINED_NEED1bitsound
snd_tick:       defb	0	; Sound variable
bit_irqstatus:	defw	0
ENDIF

IF !DEFINED_GRAYlib
	defc cpygraph = CR_GRBCopy	; CrASH FastCopy
ENDIF

IF NEED_floatpack
	INCLUDE	"float.asm"
;seed for random number generator - not used yet..
fp_seed:	defb	$80,$80,0,0,0,0
;Floating point registers...
extra:		defs	6
fa:		defs	6
fasign:		defb	0
ENDIF
