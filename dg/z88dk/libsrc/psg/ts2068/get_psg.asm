;
;	Timex 2068 specific routines
;	by Stefano Bodrato, Fall 2013
;
;	int get_psg(int reg);
;
;	Get a PSG register
;
;
;	$Id: get_psg.asm,v 1.2 2015/01/19 01:33:04 pauloscustodio Exp $
;

	PUBLIC	get_psg
	PUBLIC	psg_patch2
	PUBLIC	psg_patch3
	
get_psg:

psg_patch2:
    LD	BC,$f5
	OUT	(C),l
	inc c
	in	a,(C)
	ld	l,a
	ret
