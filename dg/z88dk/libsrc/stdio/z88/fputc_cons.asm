;
;       Small C+ Library Functions
;
;	Renamed once more and rechristened for ANSIstdio
;
;	This outputs a character to the console
;
;	1/4/2000 (Original Aug 98)
;
;
;
;	$Id: fputc_cons.asm,v 1.4 2015/01/19 01:33:22 pauloscustodio Exp $
;

                INCLUDE "stdio.def"

                PUBLIC    fputc_cons	;Print char

.fputc_cons
        ld      hl,2
        add     hl,sp
        ld      a,(hl)
        cp      13
        jr      z,putchar1
        call_oz(os_out)
	ld	l,a
	ld	h,0
        ret
.putchar1
        call_oz(gn_nln)
	ld	hl,13
	ret

