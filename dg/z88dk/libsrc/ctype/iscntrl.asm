;
;	Small C z88 Character functions
;	Written by Dominic Morris <djm@jb.man.ac.uk>
;
;	1/3/99 djm
;
;	Stylish, this returned the wrong way! (djm 17/5/99)
;
;	$Id: iscntrl.asm,v 1.5 2015/01/19 01:32:42 pauloscustodio Exp $
;

PUBLIC iscntrl

; FASTCALL

.iscntrl

   ld a,l
   ld hl,1
   cp 127
   ccf
   ret z
   cp 32
   ret c
   dec l
   ret
