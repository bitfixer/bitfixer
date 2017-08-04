init:
	lda $DD02			;load from CIA2 ddra
	ora #$04			;OR the PA2 line
	sta $DD02			;write back to DDRA
	
	lda #$FF
	sta $DD02			;set userport to output	

sendcmd:
	lda #$AA			;load command
	sta $DD01			

signal_byte_ready:
	lda $DD00			;read CIA2 pra
	ora #$04 			;raise PA2 line
	sta $DD00			;write back

wait:
	lda $00C5			;read keypress
	cmp #$3E			;Q key
	beq quit			;exit 

	lda $DD0D			;read isr
	and #$10			;check input flag
	beq wait			;wait if flag not set

signal_byte_not_ready:
	lda $DD00			;read CIA2 pra
	and #$FB			;lower PA2 line
	sta $DD00			;write back
		
quit:
	rts


	;lda #$41
	;sta $04A0
	;lda #$00
	;sta $D8A0
	;rts