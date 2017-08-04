;       Startup for NEC PC6001 computers
;
;       Stefano Bodrato - Jan 2013
;
;       If an error occurs eg break we just drop back to BASIC
;
;       $Id: pc6001_crt0.asm,v 1.3 2015/01/21 07:05:00 stefano Exp $
;



                MODULE  pc6001_crt0

;--------
; Include zcc_opt.def to find out some info
;--------
        INCLUDE "zcc_opt.def"

;--------
; Some scope definitions
;--------

        EXTERN    _main           ; main() is always external to crt0 code

        PUBLIC    cleanup         ;jp'd to by exit()
        PUBLIC    l_dcal          ;jp(hl)


        PUBLIC    _vfprintf       ;jp to the printf() core

        PUBLIC    exitsp          ;atexit() variables
        PUBLIC    exitcount

       	PUBLIC	heaplast	;Near malloc heap variables
       	PUBLIC	heapblocks

        PUBLIC    __sgoioblk      ;stdio info block

;Graphic function PUBLICS..

        PUBLIC    coords          ;Current xy position
        PUBLIC    base_graphics   ;GFX memory related variables
        ;PUBLIC    gfx_bank

       	;PUBLIC	snd_tick	;Sound variable
       	

		IF (startup=2)
			defc    myzorg  = $8437  ; PC6001 - 32k (Answer "2" to "How many pages?")
		ENDIF

		IF (startup=3)
			defc    myzorg  = $8037	 ; PC6001 - MK2 (Answer "2" to "How many pages?")
		ENDIF

		IF (startup=4)
			defc    myzorg  = $4004	 ; ROM
		ENDIF

        IF      !myzorg
			defc    myzorg  = $c437  ; PC6001 - 16K
        ENDIF


; Now, getting to the real stuff now!


		org     myzorg

start:
		;di

		;ld	a,$DD
		;out	($F0),a
		;out	($F1),a

		; on entry HL holds the current location
;IF (myzorg=$c437)
;	; if we built a 16K program and we run in a 32k environment, then let's relocate it.
;		ld	a,$c4
;		cp	h
;		jr	z,noreloc
;		; if we're still here, then HL should be = $8437
;		ld	de,$c437
;		ld	bc,$3700	; This works for programs smaller than 14k
;		ldir
;		jp	$c437+17
;noreloc:
;ENDIF

		
        ld      (start1+1),sp   ;Save entry stack

IF      STACKPTR
        ld      sp,STACKPTR
ENDIF
;ELSE
;        ld      sp,$FFFF
;ENDIF
        ;ld      hl,-64
        ;add     hl,sp
        ;ld		sp,hl
        ;ld      sp,$F000
	
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
        ld      hl,__sgoioblk+2
        ld      (hl),19 ;stdin
        ld      hl,__sgoioblk+6
        ld      (hl),21 ;stdout
        ld      hl,__sgoioblk+10
        ld      (hl),21 ;stderr
 ENDIF
ENDIF
		
        call    _main
cleanup:
;
;       Deallocate memory which has been allocated here!
;
;        push    hl
IF !DEFINED_nostreams
IF DEFINED_ANSIstdio
        EXTERN      closeall
        call    closeall
ENDIF
ENDIF
;        pop     bc
start1:
        ld      sp,0
        ;ei
        ret

l_dcal:
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
        EXTERN      vfprintf_fp
        jp      vfprintf_fp
ELSE
        IF DEFINED_complexstdio
                EXTERN      vfprintf_comp
                jp      vfprintf_comp
        ELSE
                IF DEFINED_ministdio
                        EXTERN      vfprintf_mini
                        jp      vfprintf_mini
                ENDIF
        ENDIF
ENDIF


;-----------
; Now some variables
;-----------
coords:         defw    0       ; Current graphics xy coordinates
base_graphics:  defw    0       ; Address of the Graphics map
;gfx_bank:       defb    0

IF !DEFINED_HAVESEED
		PUBLIC    _std_seed        ;Integer rand() seed
_std_seed:      defw    0       ; Seed for integer rand() routines
ENDIF

exitsp:         defw    0       ; Address of where the atexit() stack is

exitcount:      defb    0       ; How many routines on the atexit() stack

heaplast:       defw    0       ; Address of last block on heap

heapblocks:     defw    0       ; Number of blocks
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

;IF DEFINED_NEED1bitsound
;snd_tick:	defb	0	; Sound variable
;ENDIF

         defm  "Small C+ PC6001"
         defb   0

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

