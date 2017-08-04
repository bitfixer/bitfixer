;
;       Colour Genie EG2000 graphics routines
;
;       clsgraph ()  -- clear screen
;
;       Stefano Bodrato - 2015
;
;
;       $Id: clsgraph.asm,v 1.1 2015/10/28 07:18:49 stefano Exp $
;


			PUBLIC    clsgraph

			INCLUDE	"graphics/grafix.inc"


.clsgraph
	call $38a9	; FGR
	ld   a,0	; black
	call $384d	; FCLS
	ld	a,3		; green
	jp  $38da	; FCOLOU
