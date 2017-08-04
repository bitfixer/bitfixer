;
;       Jupiter ACE pseudo graphics routines
;
;       cls ()  -- clear screen
;
;       Stefano Bodrato - 2014
;
;
;       $Id: clsgraph.asm,v 1.3 2015/01/19 01:32:45 pauloscustodio Exp $
;


			PUBLIC    cleargraphics
			EXTERN     loadudg6
			EXTERN	base_graphics

			INCLUDE	"graphics/grafix.inc"


.cleargraphics
	
	ld   c,0	; first UDG chr$ to load
	ld	 b,64	; number of characters to load
	ld   hl,$2c00	; UDG area
	call loadudg6

	ld	hl,(base_graphics)
	ld	bc,maxx*maxy/6
.clean
	ld	(hl),blankch
	inc	hl
	dec	bc
	ld	a,b
	or	c
	jr	nz,clean

	ret
