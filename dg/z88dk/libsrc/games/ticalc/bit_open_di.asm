; $Id: bit_open_di.asm,v 1.6 2015/01/19 01:32:45 pauloscustodio Exp $
;
; TI calculator "Infrared port" 1 bit sound functions stub
;
; (Open sound port) and disable interrupts for exact timing
;
; Stefano Bodrato - 24/10/2001
;

    PUBLIC     bit_open_di
    EXTERN     bit_irqstatus
    EXTERN     snd_tick
    
.bit_open_di
        ld a,i		; get the current status of the irq line
        di
        push af
        
        ex (sp),hl
        ld (bit_irqstatus),hl
        pop hl
	
IF FORti82
        ld      a,@11000000	; Set W1 and R1
        out     (0),a
        ld	a,@11010100
ENDIF

IF FORti83
        ld      a,@11010000
ENDIF

IF FORti85
        ld      a,@11000000
        out	(7),a
ENDIF

IF FORti86
        ld      a,@11000000
        out	(7),a
ENDIF

        ld   (snd_tick),a

	ret

