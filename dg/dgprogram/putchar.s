        .area   _CODE
_putchar::
        ld      hl,2
        add     hl,sp
        ld      a, (#0x0140)
        ld      b, a
        ld      a,(hl)
        set	    7, a
        call    #0x00FA
        ld      a, b
        inc     a
        ld      (#0x0140), a
        ret
