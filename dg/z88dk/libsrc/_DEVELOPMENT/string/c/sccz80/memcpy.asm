
; void *memcpy(void * restrict s1, const void * restrict s2, size_t n)

SECTION code_clib
SECTION code_string

PUBLIC memcpy

EXTERN asm_memcpy

memcpy:

   pop af
   pop bc
   pop hl
   pop de
   
   push de
   push hl
   push bc
   push af
   
   jp asm_memcpy
