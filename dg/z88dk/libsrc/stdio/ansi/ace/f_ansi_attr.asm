;
; 	ANSI Video handling for the Jupiter ACE
;
; 	Text Attributes
;	m - Set Graphic Rendition
;	
;	The most difficult thing to port:
;	Be careful here...
;
;	Stefano Bodrato - Feb. 2001
;
;
;	$Id: f_ansi_attr.asm,v 1.5 2015/01/19 01:33:18 pauloscustodio Exp $
;

	PUBLIC	ansi_attr

	PUBLIC	ace_inverse
	
.ace_inverse	defb 0

.ansi_attr
        and     a
        jr      nz,noreset
        ld      (ace_inverse),a
        ret
.noreset
        cp      1
        jr      nz,nobold
	ld	a,@10000000
        ld      (ace_inverse),a
        ret
.nobold
        cp      2
        jr      z,dim
        cp      8
        jr      nz,nodim
.dim
	xor	a
        ld      (ace_inverse),a
        ret
.nodim
        cp      5
        jr      nz,nounderline
	ld	a,@10000000
        ld      (ace_inverse),a
        ret
.nounderline
        cp      24
        jr      nz,noCunderline
	xor	a
        ld      (ace_inverse),a
        ret
.noCunderline
        cp      5
        jr      nz,noblink
	ld	a,@10000000
        ld      (ace_inverse),a
        ret
.noblink
        cp      25
        jr      nz,nocblink
	xor	a
        ld      (ace_inverse),a
        ret
.nocblink
        cp      7
        jr      nz,noreverse
	ld	a,@10000000
        ld      (ace_inverse),a
        ret
.noreverse
        cp      27
        jr      nz,noCreverse
	xor	a
        ld      (ace_inverse),a
        ret
.noCreverse
        cp      8	; invisible CHAR?
        jr      nz,noinvis
        ret
.noinvis
        cp      28	; cancel invisibility
        jr      nz,nocinvis
        ret
.nocinvis
        cp      30
        jp      m,nofore
        cp      37+1
        jp      p,nofore
        ret
.nofore
        cp      40
        jp      m,noback
        cp      47+1
        jp      p,noback
        sub     40
	cp	4
	jr	c,nowmn
	ld	a,@10000000
	jr	nocry
.nowmn
	xor	a
.nocry
        ld      (ace_inverse),a
.noback
        ret
