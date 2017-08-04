;	CRT0 for the Sharp X1
;
;	Karl Von Dyson (for X1s.org)
;
;    $Id: x1_crt0.asm,v 1.7 2015/01/21 07:05:01 stefano Exp $
;

	MODULE x1_crt0

;
; Initially include the zcc_opt.def file to find out lots of lovely
; information about what we should do..
;

        INCLUDE "zcc_opt.def"

;--------
; Some scope definitions
;--------

        EXTERN    _main
        EXTERN    _x1_printf

        PUBLIC    cleanup 
        PUBLIC    l_dcal
        PUBLIC    _std_seed
        PUBLIC    _vfprintf

        PUBLIC    exitsp
        PUBLIC    exitcount

        PUBLIC    heaplast
        PUBLIC    heapblocks
        PUBLIC    __sgoioblk

; X1 stdio support variables
    PUBLIC    _x1_cursor_coords
	PUBLIC    _x1_keyboard_io

; X1 stdio support entry functions
	PUBLIC	_wait_sub_cpu

;--------
; Non-zero origins must be >=32768 (startup=2 must be used for this)
;--------

        IF      !myzorg
			IF (startup=2)
                defc    myzorg  = 32768
            ELSE
                defc    myzorg  = 0
            ENDIF
        ENDIF


        org     myzorg


;--------
; Execution starts here
;--------
start:
		di
        
IF (!DEFINED_startup | (startup=1))

if (myzorg > 0)
        defs    ZORG_NOT_ZERO
endif

        ld      sp,$FFFF
		im 1
		ei
ENDIF


IF (startup=2)

if (myzorg < 32768)
        defs    ZORG_TOO_LOW
endif

    ld      sp,$FDFF

	; re-activate IPL
	ld bc,$1D00
	xor a
	out (c),a

	ld	hl,$FE00
	push hl
	EXTERN im2_Init
	call im2_Init
	pop hl
	
	EXTERN im2_EmptyISR
	ld hl,im2_EmptyISR
	ld b,128
isr_table_fill:
	ld ($FE00),hl
	inc hl
	inc hl
	djnz isr_table_fill
	ld	hl,_kbd_isr
	ld ($FE52),hl
	
	im 2
	ei

ENDIF

	call _wait_sub_cpu
	ld bc, $1900
	ld a, $E4	; Interrupt vector set
	out (c), a
	call _wait_sub_cpu
	ld bc, $1900
	ld a, $52	; 
	out (c), a

;IF !DEFINED_x1_no_clrscr
;	EXTERN _x1_cls
;	call _x1_cls
;ENDIF

IF !DEFINED_nostreams
IF DEFINED_ANSIstdio
; Set up the std* stuff so we can be called again
        ld      hl,__sgoioblk+2
        ld      (hl),19 ;stdin
        ld      hl,__sgoioblk+6
        ld      (hl),21 ;stdout
        ld      hl,__sgoioblk+10
        ld      (hl),21 ;stderr
ENDIF
ENDIF


; INIT math identify platform
IF NEED_floatpack
        EXTERN     init_floatpack
        call    init_floatpack
ENDIF

; Optional definition for auto MALLOC init
; it assumes we have free space between the end of 
; the compiled program and the stack pointer
IF DEFINED_USING_amalloc
	ld	hl,0
	add	hl,sp
    INCLUDE "amalloc.def"
ENDIF

        call    _main

cleanup:

IF !DEFINED_nostreams
IF DEFINED_ANSIstdio
        EXTERN     closeall
        call    closeall
ENDIF
ENDIF

		push    hl				; return code

		rst 0

cleanup_exit:
		ret


IF (!DEFINED_startup | (startup=1))

        defs    56-cleanup_exit-1

if (ASMPC<>$0038)
        defs    CODE_ALIGNMENT_ERROR
endif

ENDIF

; -------------------------------------------
; Don't move the following block !
; It must be aligned when startup=1 !!

_kbd_isr:
	push af
	push bc
	;push de
	push hl
	
	ld bc, $1A01
.iiki1	in a, (c)
	and $20
	jr nz, iiki1
	ld bc, $1900
	in a, (c)
	ld hl, _x1_keyboard_io+1
	ld (hl), a
	dec hl
	ld bc, $1A01
.iiki2	in a, (c)
	and $20
	jr nz, iiki2
	ld bc, $1900
	in a, (c)
	ld (hl), a

	pop hl
	;pop de
	pop bc
	pop af
	ei
	reti

; -------------------------------------------


l_dcal:
        jp      (hl)

; Now, define some values for stdin, stdout, stderr
__sgoioblk:
IF DEFINED_ANSIstdio
        INCLUDE "stdio_fp.asm"
ELSE
        defw    -11,-12,-10
ENDIF

; Now, which of the vfprintf routines do we need?
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
		PUBLIC    _std_seed        ; Integer rand() seed
_std_seed:      defw    0                ; Seed for integer rand() routines
ENDIF

exitsp:    defw    $FDFF
exitcount: defb    0

; Heap stuff
heaplast:  defw    0
heapblocks:defw    0


; X1 stdio support variables
_x1_cursor_coords:
        defw    0
_x1_keyboard_io:
        defw    0


_wait_sub_cpu:
	ld bc, $1A01
.ii_w1	in a, (c)
	bit 6, a
	jp nz, ii_w1
	ret




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


IF NEED_floatpack
        INCLUDE         "float.asm"
;seed for random number generator - not used yet..
fp_seed:        defb    $80,$80,0,0,0,0
;Floating point registers...
extra:          defs    6
fa:             defs    6
fasign:         defb    0
ENDIF

