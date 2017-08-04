;
;	ZX IF1 & Microdrive functions
;	
;	int if1_free_sectors (int drive);
;	
;	Counts the number of free sectors in the specified drive.
;	
;	$Id: if1_free_sectors.asm,v 1.2 2015/01/19 01:33:10 pauloscustodio Exp $
;


		PUBLIC 	if1_free_sectors

filename:	defm	"!h7$"		; foo file name: it will never be written !

if1_free_sectors:
		push	hl
		rst	8
		defb 	31h		; Create Interface 1 system vars if required
		pop		hl
		
		;__FASTCALL__
		ld	a,l
		ld	($5cd6),a
		
		ld	hl,4
		ld	($5cda),hl	; length
		ld	hl,filename
		ld	($5cdc),hl	; pointer to filename

		;rst	8		; Erase if file exists ?
		;defb	24h
		
		rst	8
		defb	22h		; Open temporary 'M' channel (touch)

		xor	a
		rst	8
		defb	21h		; stop microdrive motor

		ld	l,(ix+$1a)	; load address of the associated
		ld	h,(ix+$1b)	; map into the HL register.

		ld	de,0
		ld	c,32
loop:		
		ld	a,(hl)
		ld	b,8
byteloop:
		rra
		jr	c,notfree
		inc	de
notfree:
		djnz byteloop
		inc	hl
		dec	c
		jr	nz,loop
		
		push de

		rst	8
		defb	2Ch		; Reclaim the channel
		
		pop	hl
		ret
