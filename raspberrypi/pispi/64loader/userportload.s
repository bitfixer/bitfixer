.segment	"CODE"

.proc	_main: near

.segment	"CODE"

; load bytes into program memory
; userport data is at 0xDD01

; initialize userport

; set userport to input

; store destination address
    ;lda #$00
    ;sta $00FB
    ;lda #$08
    ;sta $00FC


    ;lda #$00        ; video memory
    ;sta $00FB
    ;lda #$04
    ;sta $00FC

    ;ldy #$00
    ldx #$00
loop:
    ;lda $DD01       ;load from userport to accumulator
    ;txa
    ;inx

    ;sta ($FB), y    ;store to program memory
    ;iny

    txa
    sta $0400,x
    inx
    bne loop

loop2:
    nop
    jmp loop2

    rts

.endproc