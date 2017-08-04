;
; 	Basic I/O for RCM2/3000, use serial port or so!!
;	TODO:	 Write me!!
;
;	(HL)=char to display
;
;	No text scroll with this:
;	simply your code will STOP when the last line is reached!!
;
;	With this you'll save a bit more than 1 Kbyte.
;	Useful if you want to use the 3K expansion.
;
;
;	$Id: fputc_cons.asm,v 1.2 2015/01/21 08:09:27 stefano Exp $
;

	PUBLIC	fputc_cons
	EXTERN	rcmx000_cnvtab

	EXTERN __sendchar
			
.fputc_cons
	; extern int __LIB__ fputc(int c, FILE *fp);

	; int c is in register BC (i.e. C)
	; We ignore FILE* fp (in DE) for now

	ld a,c
		
	call __sendchar
	
	ld hl,0	

	ret

