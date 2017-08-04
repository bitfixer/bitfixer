;
; int fdgetpos(int fd, long *dump)
;
; Return position in file
;
; This returns longs even though there's no real need to do so
;
; $Id: fdgetpos.asm,v 1.4 2015/01/21 08:09:27 stefano Exp $

		PUBLIC	fdgetpos

		INCLUDE "p3dos.def"

		EXTERN	l_plong

		EXTERN	dodos


.fdgetpos
	pop	hl	;ret address
	pop	de	;where to store it
	pop	bc	;lower 8 is file handle
	push	bc
	pop	de
	push	hl
	ld	b,c
	push	de	;save store location
	ld	iy,DOS_GET_POSITION
	call	dodos
	pop	bc	;get store location back
	jr	nc,fdgetpos_store
	ld	hl,-1
	ret
.fdgetpos_store
	ld	d,0	;clear upper byte
	call	l_plong
	ld	hl,0
	ret
