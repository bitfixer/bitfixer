;       CRT0 for the Amstrad CPC family
;
;       Stefano Bodrato 8/6/2000
;
;       $Id: cpc_crt0.asm,v 1.20 2015/01/21 07:05:00 stefano Exp $
;

        MODULE  cpc_crt0


;--------
; Include zcc_opt.def to find out some info
;--------
        INCLUDE "zcc_opt.def"

;--------
; Some scope definitions
;--------

        EXTERN    _main           ;main() is always external to crt0 code

        PUBLIC    cleanup         ;jp'd to by exit()
        PUBLIC    l_dcal          ;jp(hl)

        PUBLIC    _vfprintf       ;jp to the printf() core

        PUBLIC    exitsp          ;atexit() variables
        PUBLIC    exitcount

        PUBLIC    heaplast        ;Near malloc heap variables
        PUBLIC    heapblocks

        PUBLIC    __sgoioblk      ;stdio info block

        PUBLIC    base_graphics   ;Graphical variables
        PUBLIC    coords          ;Current xy position

        PUBLIC    snd_tick        ;Sound variable
        PUBLIC	bit_irqstatus	; current irq status when DI is necessary

        PUBLIC    firmware_bc     ;Needed by the firmware interposer
        PUBLIC    firmware_af     ;Needed by the firmware interposer

;--------
; Set an origin for the application (-zorg=) default to $6000
;--------

        IF      !myzorg
                defc    myzorg  = $6000
        ENDIF   
                org     myzorg


        org     myzorg


;--------
; REAL CODE
;--------

start:
IF (startup=2)
        ld      hl,($39)        ; Original Interrupt code
        ld      (oldint),hl
        ld      hl,newint       ; Point to a null handler (increase stability)
        ld      ($39),hl
ENDIF
        di
        ld      (start1+1),sp
        ld      hl,-6530
        add     hl,sp
        ld      sp,hl
        ld      (exitsp),sp
        exx
        ld      (firmware_bc),bc        ; keep BC', otherwise crash
        exx
        ex      af,af
        push    af
        pop     bc
        ld      (firmware_af),bc        ; keep F', otherwise crash
        ex      af,af

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


; INIT math identify platform
IF NEED_floatpack
        EXTERN     init_floatpack
        call    init_floatpack
ENDIF

        call    _main

cleanup:
;
;       Deallocate memory which has been allocated here!
;

IF !DEFINED_nostreams
IF DEFINED_ANSIstdio
        EXTERN     closeall
        call    closeall
ENDIF
ENDIF
        exx
        ld      bc,(firmware_bc)        ; restore BC'
        exx
        ex      af,af
        ld      bc,(firmware_af)        ; restore F'
        push    bc
        pop     af
        ex      af,af

IF (startup=2)
        ld      hl,(oldint)
        ld      ($39),hl
ENDIF
        
start1: ld      sp,0
        ei
        ret

l_dcal: jp      (hl)

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

        
IF !DEFINED_HAVESEED
                PUBLIC    _std_seed        ;Integer rand() seed
_std_seed:       defw    0       ; Seed for integer rand() routines
ENDIF

exitsp:         defw    0       ;  atexit       
exitcount:      defb    0
base_graphics:
                defw    $C000
coords:         defw    0
firmware_bc:    defw    0
firmware_af:    defw    0

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

IF DEFINED_NEED1bitsound
snd_tick:       defb	0	; Sound variable
bit_irqstatus:	defw	0
ENDIF

                defm    "Small C+ CPC"
                defb    0

IF NEED_floatpack
        INCLUDE         "float.asm"
;init_floatpack:
;       ret

;seed for random number generator - not used yet..
fp_seed:        defb    $80,$80,0,0,0,0
;Floating point registers...
extra:          defs    6
fa:             defs    6
fasign:         defb    0

ENDIF

IF (startup=2)
newint:
        ei
        reti
oldint:
        defw 0
ENDIF
