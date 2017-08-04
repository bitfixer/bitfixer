; $Id: bit_open.asm,v 1.3 2015/01/19 01:32:45 pauloscustodio Exp $
;
; TI calculator "Infrared port" 1 bit sound functions stub
;
; void bit_open();
;
; Stefano Bodrato - 24/10/2001
;

    PUBLIC     bit_open

.bit_open

IF FORti82
        ld      a,@11000000	; Set W1 and R1
        out     (0),a
        ld	a,@11010100
ENDIF

IF FORti83
        ld      a,@11010000
ENDIF

IF FORti85
        ld      a,@11000000
        out	(7),a
ENDIF

IF FORti86
        ld      a,@11000000
        out	(7),a
ENDIF

          ret
