;#charset ascii
;#charset map " !" = 0xA0
;#charset map "#$%&" = 0xA3
;#charset map "'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~" = 0xA7

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
blah:   call _main
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

;#code _HEAP
;__sdcc_heap_start::
;.ds 1023
;#code _HEAP_END
;__sdcc_heap_end::
;.ds 1

#code _HEAP
__sdcc_heap_start::
#code _HEAP_END, _HEAP+1024
__sdcc_heap_end::

;#code PADDING
;pad:    nop
;.align 256

#CFLAGS $CFLAGS --nostdinc -I/Users/gubbish/sdcc/include
#include "printf_dg.c"
#include "putchar.s"
#include "/Users/gubbish/sdcc/lib/__ltoa.c"
#include "/Users/gubbish/sdcc/lib/__ultoa.c"
#include "/Users/gubbish/sdcc/lib/__modulong.c"
#include "/Users/gubbish/sdcc/lib/__divulong.c"
#include "dgprog.c"
;#include "/Users/gubbish/sdcc/lib/_sqrtf.c"
;#include "/Users/gubbish/sdcc/lib/___fseq.c"
;#include "/Users/gubbish/sdcc/lib/___fslt.c"
;#include "/Users/gubbish/sdcc/lib/_errno.c"
;#include "/Users/gubbish/sdcc/lib/_frexpf.c"
;#include "/Users/gubbish/sdcc/lib/___fsmul.c"
;#include "/Users/gubbish/sdcc/lib/___fsadd.c"
;#include "/Users/gubbish/sdcc/lib/___fsdiv.c"
;#include "/Users/gubbish/sdcc/lib/___fssub.c"
;#include "/Users/gubbish/sdcc/lib/___fsgt.c"
;#include "/Users/gubbish/sdcc/lib/___uint2fs.c"
;#include "/Users/gubbish/sdcc/lib/_ldexpf.c"
;#include "/Users/gubbish/sdcc/lib/__mullong.c"
;#include "/Users/gubbish/sdcc/lib/__mulint.s"
;#include "/Users/gubbish/sdcc/lib/___ulong2fs.c"
;#include "/Users/gubbish/sdcc/lib/__moduchar.s"
;#include "/Users/gubbish/sdcc/lib/__divsint.s"
;#include "/Users/gubbish/sdcc/lib/__divu8.s"
;#include "/Users/gubbish/sdcc/lib/__divu16.s"
#include "/Users/gubbish/sdcc/lib/__uitoa.c"
#include "/Users/gubbish/sdcc/lib/__moduint.s"
#include "/Users/gubbish/sdcc/lib/__divuint.s"
#include "/Users/gubbish/sdcc/lib/_malloc.c"
#include "/Users/gubbish/sdcc/lib/_free.c"
;#include "/Users/gubbish/sdcc/lib/OTHER/heap .s"
