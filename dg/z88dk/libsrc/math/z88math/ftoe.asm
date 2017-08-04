; 	Z88 Specific Maths Library
;
;	Convert double to string
;
;void ftoe(x,prec,str)
;double x ;              /* number to be converted */
;int prec ;              /* # digits after decimal place */
;char *str ;             /* output string */


		PUBLIC	ftoe

		INCLUDE	"fpp.def"

.ftoe
	ld	ix,0
	add	ix,sp

	exx
	ld	d,1		;exponential format
	ld	e,(ix+4)	;digits of d.p. may work who knows?
	ld	l,(ix+6+1)
	ld	h,(ix+6+2)
	exx
	ld	l,(ix+6+3)
	ld	h,(ix+6+4)
	ld	c,(ix+6+5)	;number
	ld	e,(ix+2)	;buffer
	ld	d,(ix+3)
	fpp(FP_STR)
	ret

	

	

