;
;	Keyboard routines for the NASCOM1/2
;	By Stefano Bodrato - 23/05/2003
;
;	getkey() Wait for keypress
;
;
;	$Id: fgetc_cons.asm,v 1.3 2015/01/19 01:33:20 pauloscustodio Exp $
;

		PUBLIC	fgetc_cons
		EXTERN	montest

.fgetc_cons

	call	montest
	jr	nz,nassys

; NASBUG 'T' monitor
.tin
	call	0c4dh
	jr	nc,tin
	
	cp	1dh
	jr	nz,notbs
	ld	a,8
.notbs
	cp	1fh
	jr	nz,notcr
	ld	a,13
.notcr
	jr	fgetcc_exit

; NASSYS monitor
.nassys
	rst	8
	
.fgetcc_exit
	ld	l,a
	ld	h,0
	ret
