;
;	CCE MC-1000 Stdio
;
;	getk() Read key status
;
;	Stefano Bodrato, 2013
;
;
;	$Id: getk.asm,v 1.5 2015/01/19 01:33:20 pauloscustodio Exp $
;

; The code at entry $c009 checks if a key has been pressed in a 7ms interval.
; If so, A and ($012E) are set to $FF and the key code is put in ($011C).
; Otherwise, A and ($012E) are set to $00.

	PUBLIC	getk

.getk
	call	$C027
	and		a
	jr		z,key_got
	ld		a,($011B)

.key_got
	ld	l,a
	ld	h,0
	ret
