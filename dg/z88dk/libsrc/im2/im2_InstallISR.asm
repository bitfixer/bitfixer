; CALLER linkage for function pointers

PUBLIC im2_InstallISR

EXTERN im2_InstallISR_callee
EXTERN ASMDISP_IM2_INSTALLISR_CALLEE

.im2_InstallISR

   pop af
   pop de
   pop hl
   push hl
   push de
   push af
   
   jp im2_InstallISR_callee + ASMDISP_IM2_INSTALLISR_CALLEE
