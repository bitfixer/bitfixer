;
; Small C z88 Misc functions
;
; sleep(time)
;
; Pause for time centiseconds
;
; djm 1/12/98
;
; If we can't have usleep we'll have csleep instead!
;
; -----
; $Id: csleep.asm,v 1.6 2015/01/21 08:09:27 stefano Exp $


                INCLUDE "time.def"

                PUBLIC    csleep
                PUBLIC    ASMDISP_CSLEEP


;csleep(int time);


.csleep
        pop     hl
        pop     bc      ;number of centi-seconds..
        push    bc
        push    hl

.asmentry

        ld      a,b
        or      c
        jr      z,csleep1
        call_oz(os_dly)
        ld      hl,1
        ret     c
.csleep1
        ld      hl,0
        ret

DEFC ASMDISP_CSLEEP = # asmentry - csleep

