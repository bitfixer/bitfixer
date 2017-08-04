;       Z88 Small C+ Run Time Library 
;       Long functions
;

                PUBLIC    l_long_le
                EXTERN     l_long_cmp




;
;......logical operations: HL set to 0 (false) or 1 (true)
;
; DE <= HL [signed]
.l_long_le
        call    l_long_cmp
        ret     c
        scf
        ret     z
        ccf
        dec     hl
        ret
