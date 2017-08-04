;       CRT0 for the Rabbit Control Module
;
;       If an error occurs (eg. out if screen) we just drop back to BASIC
;
; - - - - - - -
;
;       $Id: rcmx000_crt0.asm,v 1.8 2015/01/21 07:05:00 stefano Exp $
;
; - - - - - - -

	; TODO_KANIN Fix this!!!

	MODULE  rcmx000_crt0

;-------
; Include zcc_opt.def to find out information about us
;-------

        INCLUDE "zcc_opt.def"

;-------
; Simulate unsupported z80 instructions
;-------

        EXTERN     rcmx_cpd
        EXTERN     rcmx_cpdr
        EXTERN     rcmx_cpi
        EXTERN     rcmx_cpir
        EXTERN     rcmx_rld
        EXTERN     rcmx_rrd

;-------
; Some general scope declarations
;-------

        EXTERN    _main           ;main() is always external to crt0 code

	PUBLIC	__sendchar	;  Used by stdio
	PUBLIC    __recvchar
	
        PUBLIC    cleanup         ;jp'd to by exit()
        PUBLIC    l_dcal          ;jp(hl)

        PUBLIC    _std_seed        ;Integer rand() seed

        PUBLIC    _vfprintf       ;jp to the printf() core

        PUBLIC    exitsp          ;atexit() variables
        PUBLIC    exitcount

        PUBLIC    __sgoioblk      ;stdio info block

       	PUBLIC	heaplast	;Near malloc heap variables
	PUBLIC	heapblocks

	org 0
start:
	; On this platform we are king of the road and may use
	; any register for any purpose Wheee!!

	include "rcmx000_boot.asm"
	jp _main

IF !DEFINED_nostreams
IF DEFINED_ANSIstdio
; Set up the std* stuff so we can be called again
	ld	hl,__sgoioblk+2
	ld	(hl),19	;stdin
	ld	hl,__sgoioblk+6
	ld	(hl),21	;stdout
	ld	hl,__sgoioblk+10
	ld	(hl),21	;stderr
ENDIF
ENDIF
        call    _main	;Call user program
        
cleanup:
;
;       Deallocate memory which has been allocated here!
;
	push	hl
IF !DEFINED_nostreams
IF DEFINED_ANSIstdio
	EXTERN	closeall
	call	closeall
ENDIF
ENDIF

	pop	bc
start1:	ld	sp,0		;Restore stack to some sane value

	; Puts us back into the monitor
	call 8

l_dcal:	jp	(hl)		;Used for function pointer calls
        jp      (hl)



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
	EXTERN	vfprintf_fp
	jp	vfprintf_fp
ELSE
	IF DEFINED_complexstdio
		EXTERN	vfprintf_comp
		jp	vfprintf_comp
	ELSE
		IF DEFINED_ministdio
			EXTERN	vfprintf_mini
			jp	vfprintf_mini
		ENDIF
	ENDIF
ENDIF


;-----------
; Now some variables
;-----------

	; Here is a great place to store temp variables and stuff!!
acme:	defw 4711 			; useless arbitrarily choosen number
	defm  "Small C+ RCM2/3000",0	;Unnecessary file signature
_std_seed:
	defw 0          		; Needed for rand and srand

;-----------------------
; Floating point support
;-----------------------
IF NEED_floatpack
        INCLUDE         "float.asm"
fp_seed:        defb    $80,$80,0,0,0,0 ;FP seed (unused ATM)
extra:          defs    6               ;FP register
fa:             defs    6               ;FP Accumulator
fasign:         defb    0               ;FP register
ENDIF
