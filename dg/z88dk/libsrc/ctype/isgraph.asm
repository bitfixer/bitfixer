
PUBLIC isgraph
EXTERN asm_isgraph

.isgraph

   ld a,l
   call asm_isgraph
   ld hl,1
   ret nc
   dec l
   ret
