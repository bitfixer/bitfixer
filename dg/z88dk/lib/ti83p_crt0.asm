;	Stub for the TI 83+ calculator
;
;	Stefano Bodrato - Dec 2000
;			Feb 2000 - Speeded up the cpygraph
;
;	$Id: ti83p_crt0.asm,v 1.26 2015/01/21 07:05:00 stefano Exp $
;
; startup =
;   n - Primary shell, compatible shells
;       (Primary shell merely means it's the smallest implementation
;        for that shell, that uses full capabilities of the shell)
;
;   1  - Ion (default)
;   2  - MirageOS without quit key
;   3  -
;   4  - TSE Kernel
;   10 - asm( executable
;
;-----------------------------------------------------
; Some general PUBLICs and EXTERNs needed by the assembler
;-----------------------------------------------------

	MODULE  Ti83plus_crt0

	EXTERN	_main		; No matter what set up we have, main is
				;  always, always external to this file.

	PUBLIC	cleanup		; used by exit()
	PUBLIC	l_dcal		; used by calculated calls = "call (hl)"

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

	INCLUDE "Ti83p.def"	; ROM / RAM adresses on Ti83+[SE]
	INCLUDE	"zcc_opt.def"	; Receive all compiler-defines
	
;-----------------------------
;2 - MirageOS without quit key
;-----------------------------
IF (startup=2)
	DEFINE MirageOS			;Used by greyscale interrupt etc.
	DEFINE NOT_DEFAULT_SHELL	;Else we would use Ion
	;org	$9D93			;Origin
	;defb	$BB,$6D			;Compiled AsmPrgm token
	org	$9D95	;We use Bin2var
	ret				;So TIOS wont run the program
	defb	1			;Identifier as MirageOS program
	DEFINE NEED_mirage_icon
	INCLUDE	"zcc_opt.def"
	UNDEFINE NEED_mirage_icon
 IF !DEFINED_NEED_mirage_icon
	defb	@01111000,@00000000	;Picture of a map with "C+" on it
	defb	@10000100,@00000000
	defb	@10000011,@11111100	;15x15 button
	defb	@10000000,@00000010
	defb	@10011111,@00000010
	defb	@10111111,@00000010
	defb	@10110000,@00110010
	defb	@10110000,@01111010
	defb	@10110000,@01111010
	defb	@10110000,@00110010
	defb	@10111111,@00000010
	defb	@10011111,@00000010
	defb	@10000000,@00000010
	defb	@10000000,@00000010
	defb	@01111111,@11111100
 ENDIF
	DEFINE NEED_name
	INCLUDE	"zcc_opt.def"		; Get namestring from zcc_opt.def
	UNDEFINE NEED_name
 IF !DEFINED_NEED_name
	defm	"Z88DK Small C+ Program"
 ENDIF
	defb	$0			; Termination zero
	im	1			; Disable MirageOS tasker interrupt...
ENDIF

;--------------
;4 - TSE Kernel
;--------------
IF (startup = 4)
	DEFINE TSE
	DEFINE NOT_DEFAULT_SHELL
	org	$9D94
	ret
	defm	"TSE"
	defb	1
	defm	" "
	DEFINE NEED_name
	INCLUDE	"zcc_opt.def"
	UNDEFINE NEED_name
 IF !DEFINED_NEED_name
	defm	"Z88DK Small C+ Program"
 ENDIF
	defb	$0
;-----------------------------------------------------------------------------
; External Data Required for virtual stack. I've set this to 400
; (the normal size of the Ti83+ stack). You can use #pragma to set
; a userdefined  value (RECOMMENDED):
; #pragma output StackNeeded = nnn;
;-----------------------------------------------------------------------------
 IF !DEFINED_StackNeeded
	defw	400
 ELSE
	defw	DEFINED_StackNeeded
 ENDIF
ENDIF

;--------------------
;10 - asm( executable
;--------------------
IF (startup=10)
	DEFINE ASM
	DEFINE NOT_DEFAULT_SHELL
	org	$9D93
	defb	$BB,$6D
ENDIF

;-----------------
;1 - Ion (default)
;-----------------
IF !NOT_DEFAULT_SHELL
	DEFINE Ion
	org	$9D95
	;org	$9D93
	;defb         $BB,$6D
	ret
	jr	nc,start
	DEFINE NEED_name
	INCLUDE	"zcc_opt.def"
	UNDEFINE NEED_name
 IF !DEFINED_NEED_name
	defm	"Z88DK Small C+ Program"
 ENDIF
	defb	$0
ENDIF


;-------------------------------------
; End of header, begin of startup part
;-------------------------------------
start:
IF DEFINED_GimmeSpeed
	ld	a,1		; switch to 15MHz (extra fast)
	rst	28		; bcall(SetExSpeed)
	defw	SetExSpeed	;
ENDIF				;
	ld	hl,0		;
	add	hl,sp		;
	ld	(start1+1),hl	;
IF !DEFINED_atexit		; Less stack use
	ld	hl,-6		; 3 pointers (more likely value)
	add	hl,sp		;
	ld	sp,hl		;
	ld	(exitsp),sp	;
ELSE				;
	ld	hl,-64		; 32 pointers (ANSI standard)
	add	hl,sp		;
	ld	sp,hl		;
	ld	(exitsp),sp	;
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
 IF DEFINED_GimmeSpeed
	INCLUDE "gray83pSE.asm"	; 15MHz grayscale interrupt
 ELSE
	INCLUDE	"gray83p.asm"		;  6MHz grayscale interrupt
 ENDIF
ELSE
	INCLUDE	"intwrap83p.asm"	; Interrupt Wrapper
ENDIF

	im	2		; enable IM2 interrupt
	call	_main		; call main()
cleanup:			; exit() jumps to this point
	ld	iy,_IY_TABLE	; Restore flag pointer
	im	1		;
IF DEFINED_GimmeSpeed		;
	xor	a		; Switch to 6MHz (normal speed)
	rst	28		; bcall(SetExSpeed)
	defw	SetExSpeed	;
ENDIF				;
start1:	ld	sp,0		; Restore SP
IF TSE				; TSE Kernel
	call	_tseForceYield	; Task-switch back to shell (can return...)
	jp	start		; begin again if needed...
ENDIF				;
tiei:	ei			;
IF DEFINED_GRAYlib		;
cpygraph:
ENDIF				;
tidi:	ret			;

;----------------------------------------
; End of startup part, routines following
;----------------------------------------
l_dcal:
	jp	(hl)		; used as "call (hl)"


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

; Heap stuff
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

; mem stuff
base_graphics:	defw	plotSScreen
coords:		defw	0

IF DEFINED_NEED1bitsound
snd_tick:       defb	0	; Sound variable
bit_irqstatus:	defw	0
ENDIF

IF !DEFINED_GRAYlib
 IF DEFINED_GimmeSpeed
cpygraph:
	call	$50		; bjump(GrBufCpy)
	defw	GrBufCpy	; FastCopy is far too fast at 15MHz...
 ELSE
  IF Ion
	defc	cpygraph = $966E+80+15	; Ion FastCopy call
  ENDIF
  IF MirageOS
	defc	cpygraph = $4092	; MirageOS FastCopy call
  ENDIF
  IF TSE
	defc	cpygraph = $8A3A+18	; TSE FastCopy call
  ENDIF
  IF ASM
cpygraph:
;(ion)FastCopy from Joe Wingbermuehle
	di
	ld	a,$80				; 7
	out	($10),a				; 11
	ld	hl,plotSScreen-12-(-(12*64)+1)		; 10
	ld	a,$20				; 7
	ld	c,a				; 4
	inc	hl				; 6 waste
	dec	hl				; 6 waste
fastCopyAgain:
	ld	b,64				; 7
	inc	c				; 4
	ld	de,-(12*64)+1			; 10
	out	($10),a				; 11
	add	hl,de				; 11
	ld	de,10				; 10
fastCopyLoop:
	add	hl,de				; 11
	inc	hl				; 6 waste
	inc	hl				; 6 waste
	inc	de				; 6
	ld	a,(hl)				; 7
	out	($11),a				; 11
	dec	de				; 6
	djnz	fastCopyLoop			; 13/8
	ld	a,c				; 4
	cp	$2B+1				; 7
	jr	nz,fastCopyAgain		; 10/1
	ret					; 10
  ENDIF
 ENDIF
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
