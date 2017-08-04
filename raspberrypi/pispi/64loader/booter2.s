.segment    "LOADER"

    .byte   $01,$08
    .byte   $0c,$08,$d0,$07,$9e,$20,$32,$30,$36,$34,$00,$00,$00	; SYS 2064 in "Basic-Code"

.segment	"RODATA"

_program:	;ml program here


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

