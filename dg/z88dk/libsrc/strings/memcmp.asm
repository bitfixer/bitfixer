; CALLER linkage for function pointers

PUBLIC memcmp
EXTERN memcmp_callee
EXTERN ASMDISP_MEMCMP_CALLEE

.memcmp

   pop af
   pop bc
   pop de
   pop hl
   push hl
   push de
   push bc
   push af
   
   jp memcmp_callee + ASMDISP_MEMCMP_CALLEE
