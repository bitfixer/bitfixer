;
;       Small C+ Runtime Library
;
;       Random number generator for seed
;	generic version, 
;
;       void randomize() - randomize the seed for rand()
;
; -----
; $Id: randomize.asm,v 1.4 2015/01/19 01:33:22 pauloscustodio Exp $

PUBLIC randomize
EXTERN _std_seed
EXTERN cleanup

; you must declare an integer named "std_seed" in your
; main.c file to hold the seed.
;
; int std_seed;

.randomize

   ld	hl,0
   add	hl,sp
   call	agarble
   ld	e,a
   
   ld	hl,cleanup	; we fall into the CRT0 stub
   ld	bc,1024
   sbc	hl,bc
   call	agarble
   
   ld	h,a

IF FORrcmx000
   ld	a,eir
ELSE
   ld	a,r
ENDIF

   xor	e
   ld	l,a
   
   ld (_std_seed),hl
   ret


.agarble
   ld	bc,$FF04
.sloop
   add	(hl)
   inc	hl
   djnz sloop
   dec	c
   jr	nz,sloop
   ret