#charset ascii
#charset map " !" = 0xA0
#charset map "#$%&" = 0xA3
#charset map "'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~" = 0xA7

#target bin
#code HEADER,0x0100
.byte 0x53
.byte 0x53
.byte 0xC3
.byte 0x00
.byte 0x00
.byte 0xC3
.byte 0x00
.byte 0x00
.byte 0xC3
.byte 0x00
.byte 0x00
.byte 0xC3
.byte 0x00
.byte 0x00
.byte 0xC3
.byte 0x00
.byte 0x00
.byte 0xC3
.byte 0x00
.byte 0x00
.byte 0xC3
.byte 0x00
.byte 0x00
.byte 0x1F
.byte 0x00
.byte 0x01
.byte 0xFF  ; end addr L. Will be replaced by dglength.py
.byte 0x05  ; end addr H
.byte 0x00
.byte 0x00
.byte 0x00
.byte 0x00
#code OS,0x0120
blah:   call 0x00E6
        call 0x00F8     ; insert blank space
        call 0x00F8
        call 0x00F8
        call 0x00F8
        call 0x00F8
        ;ld a,(TEXT)
        ;call 0x00FA     ; print a character
        ;ld a,(TEXT+1)
        ;call 0x00FA
        ;ld a,(TEXT+2)
        ;call 0x00FA
        call _bbcc
        ;ld a,(0x0400)
        ;ld a,(TEXT+3)
        ;call 0x00FA
loop:   jp loop
#code TEXT
.text "XYCD"
.align 0x0500
#code JUMPER,0x0500
go:     jp blah
;.align 256

#code _GSINIT
#code _HOME
#code _CODE
#code _CABS,*,0
#code _GSFINAL,*,0
#code _INITIALIZER
#data _DATA
#data _INITIALIZED
#data _DABS,*,0

;#code PADDING
;pad:    nop
;.align 256

#CFLAGS $CFLAGS --nostdinc -I/Users/gubbish/sdcc/include
#include "dgprog.c"
#include "/Users/gubbish/sdcc/lib/_sprintf.c"
#include "/Users/gubbish/sdcc/lib/_put_char_to_string.c"
#include "/Users/gubbish/sdcc/lib/__print_format.c"
#include "/Users/gubbish/sdcc/lib/__sdcc_call_hl.s"
#include "/Users/gubbish/sdcc/lib/___fslt.c"
#include "/Users/gubbish/sdcc/lib/___fsgt.c"
#include "/Users/gubbish/sdcc/lib/___fsdiv.c"
#include "/Users/gubbish/sdcc/lib/___fsmul.c"
#include "/Users/gubbish/sdcc/lib/___fsadd.c"
#include "/Users/gubbish/sdcc/lib/___fs2ulong.c"
#include "/Users/gubbish/sdcc/lib/___ulong2fs.c"
#include "/Users/gubbish/sdcc/lib/___fssub.c"
#include "/Users/gubbish/sdcc/lib/__modulong.c"
#include "/Users/gubbish/sdcc/lib/__divulong.c"
#include "/Users/gubbish/sdcc/lib/__divuschar.s"
#include "/Users/gubbish/sdcc/lib/__moduschar.s"
#include "/Users/gubbish/sdcc/lib/_strlen.s"
#include "/Users/gubbish/sdcc/lib/__mullong.c"
#include "/Users/gubbish/sdcc/lib/__div_signexte.s"
;#include "/Users/gubbish/sdcc/lib/__get_remainder.s"
;#include "/Users/gubbish/sdcc/lib/__div16.s"
#include "/Users/gubbish/sdcc/lib/__mulint.s"
#include "/Users/gubbish/sdcc/lib/__divu16.s"
