.segment    "LOADER"

    .byte   $01,$08
    .byte   $0c,$08,$d0,$07,$9e,$20,$32,$30,$36,$34,$00,$00,$00	; SYS 2064 in "Basic-Code"

.segment	"RODATA"

_test:
	.byte	$00
	.byte	$01
	.byte	$02
	.byte	$03

; ---------------------------------------------------------------
; void __near__ main (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_main: near

.segment	"CODE"

    ldx #$00
loop:
    lda #$00
    sta $D800,x
	lda _test,x
    sta $C000,x
    sta $04A0,x
    inx
    cpx #$04
    bne loop
	rts

.endproc

