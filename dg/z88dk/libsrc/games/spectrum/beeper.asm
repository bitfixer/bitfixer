; $Id: beeper.asm,v 1.4 2013/10/21 14:23:45 stefano Exp $
;
; ZX Spectrum 1 bit sound functions
;
; Stefano Bodrato - 28/9/2001
;

    XLIB     beeper

;
; Spectrum beeper routine!!
; HL=duration
; DE=frequency
;
; Comments got from: http://www.wearmouth.demon.co.uk/scsra.htm
; ----------------------------------------------------------------
;
; Outputs a square wave of given duration and frequency
; to the loudspeaker.
;   Enter with: DE = #cycles - 1
;               HL = tone period as described next
;
; The tone period is measured in T states and consists of
; three parts: a coarse part (H register), a medium part
; (bits 7..2 of L) and a fine part (bits 1..0 of L) which
; contribute to the waveform timing as follows:
;
;                          coarse    medium       fine
; duration of low  = 118 + 1024*H + 16*(L>>2) + 4*(L&0x3)
; duration of hi   = 118 + 1024*H + 16*(L>>2) + 4*(L&0x3)
; Tp = tone period = 236 + 2048*H + 32*(L>>2) + 8*(L&0x3)
;                  = 236 + 2048*H + 8*L = 236 + 8*HL
;
; As an example, to output five seconds of middle C (261.624 Hz):
;   (a) Tone period = 1/261.624 = 3.822ms
;   (b) Tone period in T-States = 3.822ms*fCPU = 13378
;         where fCPU = clock frequency of the CPU = 3.5MHz
;   (c) Find H and L for desired tone period:
;         HL = (Tp - 236) / 8 = (13378 - 236) / 8 = 1643 = 0x066B
;   (d) Tone duration in cycles = 5s/3.822ms = 1308 cycles
;         DE = 1308 - 1 = 0x051B
;
; The resulting waveform has a duty ratio of exactly 50%.
; 
; ----------------------------------------------------------------

	XREF  call_rom3
    LIB      bit_open_di
    LIB      bit_close_ei

.beeper
     call    bit_open_di
	 call    call_rom3
	 defw    949
	 di
	 call    bit_close_ei
	 ret
