.segment    "LOADER"

    .byte   $01,$08
    .byte   $0c,$08,$d0,$07,$9e,$20,$32,$30,$36,$34,$00,$00,$00	; SYS 2064 in "Basic-Code"

.segment	"RODATA"

_program:
	.byte	$00		;__program insert


; ---------------------------------------------------------------
; void __near__ main (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_main: near

.segment	"CODE"

    ldy _program
    ldx #$00
loop:
	lda _program+1,x
    sta $C000,x
    inx
    dey
    bne loop
	rts

.endproc

