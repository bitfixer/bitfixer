.segment    "LOADER"

    .byte   $01,$08
    .byte   $0c,$08,$d0,$07,$9e,$20,$32,$30,$36,$34,$00,$00,$00	; SYS 2064 in "Basic-Code"

.segment	"RODATA"

_program:	;__program insert
	.byte	$a9
	.byte	$00
	.byte	$85
	.byte	$fb
	.byte	$a9
	.byte	$04
	.byte	$85
	.byte	$fc
	.byte	$a0
	.byte	$00
	.byte	$ad
	.byte	$01
	.byte	$dd
	.byte	$91
	.byte	$fb
	.byte	$c8
	.byte	$d0
	.byte	$f8


; ---------------------------------------------------------------
; void __near__ main (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_main: near

.segment	"CODE"

    ldx #$00
loop:
	lda _program,x
    sta $C000,x
    inx
    cpx #$28
    bne loop
	rts

.endproc

