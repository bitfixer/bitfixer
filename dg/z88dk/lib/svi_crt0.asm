;       Spectravideo SVI CRT0 stub
;
;       Stefano Bodrato - Apr. 2001
;
;       $Id: svi_crt0.asm,v 1.13 2015/01/21 07:05:00 stefano Exp $
;


		MODULE  svi_crt0

;
; Initially include the zcc_opt.def file to find out lots of lovely
; information about what we should do..
;

		INCLUDE "zcc_opt.def"

;--------
; Some scope definitions
;--------

		EXTERN    _main

		PUBLIC    cleanup
		PUBLIC    l_dcal

		PUBLIC    _std_seed

		PUBLIC	snd_tick	; Sound variable
		PUBLIC	bit_irqstatus	; current irq status when DI is necessary

		PUBLIC	_vfprintf

		PUBLIC    exitsp
		PUBLIC    exitcount

		PUBLIC	heaplast	; Near malloc heap variables
		PUBLIC	heapblocks

		PUBLIC    __sgoioblk

		; Graphics stuff
		PUBLIC	pixelbyte	; Temp store for non-buffered mode
		PUBLIC    base_graphics   ; Graphical variables
		PUBLIC    coords          ; Current xy position

		; MSX platform specific stuff
		;
		PUBLIC    msxbios
		;; PUBLIC    brksave



; Now, getting to the real stuff now!


        org     34816

start:
        ld      hl,0
        add     hl,sp
        ld      (start1+1),hl
        ld      hl,-64
        add     hl,sp
        ld      sp,hl
        ld      (exitsp),sp

; Optional definition for auto MALLOC init
; it assumes we have free space between the end of 
; the compiled program and the stack pointer
	IF DEFINED_USING_amalloc
		INCLUDE "amalloc.def"
	ENDIF

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
	call	$53		; Hide function key menu
        call    _main
	
cleanup:
;
;       Deallocate memory which has been allocated here!
;

IF !DEFINED_nostreams
IF DEFINED_ANSIstdio
	EXTERN	closeall
	call	closeall
ENDIF
ENDIF

start1:
        ld      sp,0

	ld	ix,$3768	; TOTEXT - force text mode on exit
	call	msxbios
        ret

l_dcal:
        jp      (hl)

; Now, define some values for stdin, stdout, stderr

__sgoioblk:
IF DEFINED_ANSIstdio
	INCLUDE	"stdio_fp.asm"
ELSE
        defw    -11,-12,-10
ENDIF


; Now, which of the vfprintf routines do we need?


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


; ---------------
; Misc Variables
; ---------------
;;defltdsk:       defb    0	; Default disc
base_graphics:  defw    0	; Location of current screen buffer
coords:         defw    0       ; Current graphics xy coordinates
pixelbyte:	defb	0


IF DEFINED_NEED1bitsound
snd_tick:       defb	0	; Sound variable
bit_irqstatus:	defw	0
ENDIF

;Seed for integer rand() routines
IF !DEFINED_HAVESEED
		PUBLIC    _std_seed        ;Integer rand() seed
_std_seed:      defw    0       ; Seed for integer rand() routines
ENDIF


;Atexit routine

exitsp:
                defw    0
exitcount:
                defb    0

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


; ---------------
; MSX specific stuff
; ---------------

; Safe BIOS call
msxbios:
	push	ix
	ret


; Signature in resulting binary

         defm  "Small C+ SVI"
	 defb	0


;All the float stuff is kept in a different file...for ease of altering!
;It will eventually be integrated into the library
;
;Here we have a minor (minor!) problem, we've no idea if we need the
;float package if this is separated from main (we had this problem before
;but it wasn't critical..so, now we will have to read in a file from
;the directory (this will be produced by zcc) which tells us if we need
;the floatpackage, and if so what it is..kludgey, but it might just work!
;
;Brainwave time! The zcc_opt file could actually be written by the
;compiler as it goes through the modules, appending as necessary - this
;way we only include the package if we *really* need it!

IF NEED_floatpack
        INCLUDE         "float.asm"

;seed for random number generator - not used yet..
fp_seed:        defb    $80,$80,0,0,0,0
;Floating point registers...
extra:          defs    6
fa:             defs    6
fasign:         defb    0

ENDIF

