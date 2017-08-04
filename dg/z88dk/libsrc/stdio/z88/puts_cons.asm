;
;       Small C+ Z88 Internal Routine
;       Puts a string to the console - mapping \n to \n\l as we
;       go and appending \n\l to the end of the line
;
;	Non standard (for short programs)
;
;       djm 2/4/99
;
;
;	$Id: puts_cons.asm,v 1.4 2015/01/19 01:33:22 pauloscustodio Exp $
;

                INCLUDE "stdio.def"

                PUBLIC    puts_cons


.puts_cons
	pop	bc
	pop	hl
	push	hl
	push	bc
.putconsole0
        ld      a,(hl)
        and     a
        jr      z,putconsole_out
        cp      13
        jr      z,putconsole_nl
        call_oz(os_out)
.putconsole_lp
        inc     hl
        jr      putconsole0
.putconsole_out
        call_oz(gn_nln)
        ret
.putconsole_nl
        call_oz(gn_nln)
        jr      putconsole_lp
                
