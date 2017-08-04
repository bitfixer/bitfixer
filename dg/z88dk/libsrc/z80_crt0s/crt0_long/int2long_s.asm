;       Small C+ Z88 Support Library
;
;       Convert signed int to long

                PUBLIC    l_int2long_s

; If MSB of h sets de to 255, if not sets de=0

.l_int2long_s
        ld      de,0
        bit     7,h
        ret     z
        dec     de
        ret

