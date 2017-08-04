;
; Sprite Rendering Routine
; original code by Patrick Davidson (TI 85)
; modified by Stefano Bodrato - Jan 2001
;
; PC6001 version
;
;
; $Id: putsprite.asm,v 1.2 2015/01/19 01:32:51 pauloscustodio Exp $
;

	PUBLIC    putsprite
	EXTERN	cpygraph
	EXTERN	pixeladdress

; coords: d,e (vert-horz)
; sprite: (ix)


.offsets_table
         defb	128,64,32,16,8,4,2,1

.putsprite

        ld      hl,2   
        add     hl,sp
        ld      e,(hl)
        inc     hl
        ld      d,(hl)  ;sprite address
	push	de
	pop	ix

        inc     hl
        ld      e,(hl)  
 	inc	hl
        inc     hl
        ld      d,(hl)	; x and y coords

	inc	hl

        inc     hl
        ld      a,(hl)  ; and/or/xor mode
        ld	(ortype+1),a	; Self modifying code
        ld	(ortype2+1),a	; Self modifying code

        inc     hl
        ld      a,(hl)
        ld	(ortype),a	; Self modifying code
        ld	(ortype2),a	; Self modifying code

	ld	h,d
	ld	l,e

	call	pixeladdress
	xor	7
         ld       hl,offsets_table
         ld       c,a
         ld       b,0
         add      hl,bc
         ld       a,(hl)
         ld       (wsmc1+1),a
         ld       (wsmc2+1),a
	 ld       (_smc1+1),a
	ld	h,d
	ld	l,e

	ld	a,(ix+0)
	cp	9
	jr	nc,putspritew

         ld       d,(ix+0)
         ld       b,(ix+1)
._oloop  push     bc                ;Save # of rows
         push     hl                ;Save screen address
         ld       b,d               ;Load width
         ld       c,(ix+2)          ;Load one line of image
         inc      ix
._smc1   ld       a,1               ;Load pixel mask
._iloop  sla      c                 ;Test leftmost pixel
         jr       nc,_noplot        ;See if a plot is needed
         ld       e,a

.ortype
	nop	; changed into nop / cpl
         nop	; changed into and/or/xor (hl)
         ld       (hl),a
         ld       a,e
._noplot rrca
	 rrca
         jr       nc,_notedge       ;Test if edge of byte reached
         inc      hl                ;Go to next byte
._notedge djnz     _iloop
         pop      hl                ;Restore address
         ld       bc,32      ;Go to next line
         add      hl,bc
         pop      bc                ;Restore data
         djnz     _oloop
         ret
         

.putspritew
         ld       d,(ix+0)
         ld       b,(ix+1)        
.woloop  push     bc                ;Save # of rows
         push     hl                ;Save screen address
         ld       b,d               ;Load width
         push	de
         ld       c,(ix+2)          ;Load one line of image
         inc      ix
         ld	d,2
.wsmc1    
	 ld	a,1
.wiloop  sla      c                 ;Test leftmost pixel
         jr       nc,wnoplot         ;See if a plot is needed
         ld       e,a

.ortype2
	nop	; changed into nop / cpl
         nop	; changed into and/or/xor (hl)
         ld       (hl),a
         ld       a,e
.wnoplot rrca
	 rrca
         jr       nc,wnotedge        ;Test if edge of byte reached
         inc      hl                ;Go to next byte
.wnotedge
.wsmc2
	cp       1
	jr	nz,nowover_1
	dec	d
        jr       z,wover_1
.nowover_1

         djnz     wiloop
         pop	de
         pop      hl                ;Restore address
         ld       bc,32		;Go to next line
         add      hl,bc
         pop      bc                ;Restore data
         djnz     woloop
         ret

.wover_1
         ld       c,(ix+2)
         inc      ix
         djnz     wiloop
         dec      ix
         pop      hl
         ld       bc,32
         add      hl,bc
         pop      bc
         djnz     woloop
         ret

