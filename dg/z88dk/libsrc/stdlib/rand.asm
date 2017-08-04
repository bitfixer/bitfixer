;
;       Small C+ Runtime Library
;
;       Random number generator
;
;       int rand(void)
;
;       Liberated from ticalc.org, mods to standard z80 by djm 11/4/99
;
; -----
; $Id: rand.asm,v 1.8 2015/01/19 01:33:22 pauloscustodio Exp $

; you must declare an integer C variable "std_seed" to hold the
; 16-bit seed in your main.c file
;
; int std_seed;

PUBLIC rand
EXTERN _std_seed

;       My contribution to randon number generators     (by Risto Jrvinen)
;       -------------------------------------------
;
;   Here's two simple 16-bit random number generators.  I originally made
; them on PC but since ZShell doesn't support ROM-random generator I converted
; them to Z80.
;   My generators were kind of quick'n'dirty hacks on PC, but acceptable on
; Z80.  Their advantage was their small size, but on Z80-assembler they are
; giant-sized.  If you (too) think that these generators are wasting TI's
; resources use random generators provided by Chris Busch (rand.asm found on
; many ZShell-sites).
;   I still use the first generator with demo coding (on PC).  It uses two
; 16-bit seeds.  It rotates first seed three bits left and adds some trash
; bits then it rotates second seed (first seed and %1111) bits left and adds
; some trash bits.  Easy and compact on PC, quite a gonzo-routine on Z80. This
; could be considered as resource wasting on TI.  The second generator uses
; only the first seed of the first generator.
;   Both generators start with zeroed seed(s).  If you wish to use randoms for
; anything make your program save & restore seed(s) upon leaving & entry.

;       16-bit seed random generator
;       ----------------------------
;  Faster than previous routine.
;  Uses only one 16-bit variable, seed.
;  Read and cut the result out of variable seed.

.rand

   ld   hl,(_std_seed)
   ld   a,h
   add  a,a                     ;Set highest bit of seed to carry
   rl   l                       ;rotate L left (C<=L<=C)
   rl   h                       ;rotate H left (C<=L<=C)
   add  a,a                     ;Set second highest bit of seed to carry
   rl   l
   rl   h
   add  a,a
   rl   l
   rl   h
   ld   bc,$7415
   add  hl,bc                   ;Add $7415 to HL
   ld   (_std_seed),hl
   res     7,h			;force to be +ve
   ret
