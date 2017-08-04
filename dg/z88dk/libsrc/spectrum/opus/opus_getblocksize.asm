;
;	ZX Spectrum OPUS DISCOVERY specific routines
;
;	Stefano Bodrato - Jun. 2006
;
;	int opus_getblocksize (int drive);
;	
;	$Id: opus_getblocksize.asm,v 1.4 2015/01/19 01:33:10 pauloscustodio Exp $
;


		PUBLIC 	opus_getblocksize

		EXTERN	opus_rommap

		EXTERN	P_DEVICE
		

opus_getblocksize:
		
		ld	ix,2
		add	ix,sp

		ld	hl,-1

		ld	a,(ix+0)	; drive
		and	a		; drive no. = 0 ?
		ret	z		; yes, return -1
		dec	a
		cp	5		; drive no. >5 ?
		ret	nc		; yes, return -1

		call	opus_rommap
		;call	$1708		; Page in the Discovery ROM
		ld	a,(ix+0)	; drive
		ld	bc,$0400	; inquire disk
		call	P_DEVICE
		call	$1748		; Page out the Discovery ROM

		ld	h,b		; block size
		ld	l,c
		ret
		
