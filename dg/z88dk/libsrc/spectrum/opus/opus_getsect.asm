;
;	ZX Spectrum OPUS DISCOVERY specific routines
;
;	Stefano Bodrato - 2014
;
;	Get a sector.
;	A standard 178K Opus disk has 0..718 sectors
;	Each sector is 256 bytes long
;
;	int opus_getsect(int drive, int sector, char * buffer); 
;
;	$Id: opus_getsect.asm,v 1.3 2015/01/19 01:33:10 pauloscustodio Exp $
;

PUBLIC opus_getsect
;XREF ASMDISP_OPUS_GETSECT_CALLEE
EXTERN opus_getsect_callee
EXTERN opus_getsect_asmentry

.opus_getsect

	pop af
	pop de		; buffer location
	pop hl		; sector number
	pop bc		; c=drive#
	push bc		; c=drive#
	push hl		; sector number
	push de		; buffer location
	push af

	jp	opus_getsect_asmentry
;   jp ASMDISP_OPUS_GETSECT_CALLEE + opus_getsect_callee
