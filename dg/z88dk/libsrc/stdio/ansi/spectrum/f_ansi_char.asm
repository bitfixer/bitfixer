;
;       Spectrum C Library
;
; 	ANSI Video handling for ZX Spectrum
;
; 	Handles colors referring to current PAPER/INK/etc. settings
;
;	** alternate (smaller) 4bit font capability: 
;	** use the -DPACKEDFONT flag
;	** ROM font -DROMFONT
;
;	set it up with:
;	.text_cols	= max columns
;	.text_rows	= max rows
;	.DOTS+1		= char size
;	.font		= font file
;
;	Display a char in location (ansi_ROW),(ansi_COLUMN)
;	A=char to display
;
;
;	$Id: f_ansi_char.asm,v 1.9 2015/01/19 01:33:19 pauloscustodio Exp $
;

	PUBLIC	ansi_CHAR
	
	EXTERN	ansi_ROW
	EXTERN	ansi_COLUMN

	PUBLIC	text_cols
	PUBLIC	text_rows
	
; Dirty thing for self modifying code
	PUBLIC	INVRS	

IF A128COL
.text_cols   defb 128
ENDIF

IF A80COL
.text_cols   defb 80
ENDIF

IF A85COL
.text_cols   defb 85
ENDIF

IF A64COL
.text_cols   defb 64
ENDIF

IF A51COL
.text_cols   defb 51
ENDIF

IF A42COL
.text_cols   defb 42
ENDIF

IF A40COL
.text_cols   defb 40
ENDIF

IF A36COL
.text_cols   defb 36
ENDIF

IF A32COL
.text_cols   defb 32
ENDIF

IF A28COL
.text_cols   defb 28
ENDIF

IF A24COL
.text_cols   defb 24
ENDIF

.text_rows   defb 24

.ansi_CHAR
  ld (char+1),a
  ld a,(ansi_ROW)       ; Line text position
  push af
  and 24
  ld d,a
  pop af
  and 7
  rrca
  rrca
  rrca
  ld e,a
  ld hl,16384
  add hl,de
  ld (RIGA+1),hl
;  xor a
  ld hl,DOTS+1
  ld b,(hl)
  ld hl,0
  ld a,(ansi_COLUMN)       ; Column text position
  ld e,a
  ld d,0
  or d
  jr z,ZCL
.LP
  add hl,de
  djnz LP
  ld b,3
.LDIV
  srl h
  rr l
  rra
  djnz LDIV
.ZCL
; Added for color handling
  push hl
  push af
  ld de,22528-32
  add hl,de
  ld a,(ansi_ROW)
  inc a
  ld de,32
.CLP
  add hl,de
  dec a
  jr nz,CLP
  ld a,(23693)  ;Current color attributes
  ld (hl),a
  pop af
  pop hl
; end of color handling

  ld b,5
.RGTA
  srl a
  djnz RGTA
  ld (PRE+1),a
  ld e,a
  ld a,(DOTS+1)
  add a,e
  ld e,a
  ld a,16
  sub e
.NOC
  ld (POST+1),a
.RIGA           ; Location on screen
  ld de,16384
  add hl,de
  push hl
  pop ix
.char
  ld b,'A'      ; Put here the character to be printed

IF ROMFONT
  ld hl,15360
ELSE
	IF PACKEDFONT
	  xor	a
	  rr	b
	  jr	c,even
	  ld	a,4
	.even
	  ld	(ROLL+1),a
	  ld	hl,font-128
	ELSE
	  ld hl,font-256
	ENDIF
ENDIF

  ld de,8

.LFONT
  add hl,de
  djnz LFONT
  ld de,256
  ld c,8
.PRE
  ld b,4
  rl (ix+1)
  rl (ix+0)
  inc b
  dec b
  jr z,DTS
.L1
  rl (ix+1)
  rl (ix+0)
  djnz L1
.DTS
  ld a,(hl)
  
IF ROMFONT
	; nothing here !
ELSE
	IF PACKEDFONT
	.ROLL
	  jr INVRS
	  rla
	  rla
	  rla
	  rla
	ENDIF
ENDIF

.INVRS
;  cpl           ; Set to NOP to disable INVERSE
  nop

; Underlined text handling
  dec c
;  jr nz,UNDRL   ; Set to JR UNDRL to disable underlined text (loc. INVRS+2)
  jr UNDRL
  ld a,255
.UNDRL
  inc c
; end of underlined text handling

.DOTS
IF A128COL
  ld b,2
ENDIF
IF A80COL
  ld b,3
ENDIF
IF A85COL
  ld b,3
ENDIF
IF A64COL
  ld b,4
ENDIF
IF A51COL
  ld b,5
ENDIF
IF A42COL
  ld b,6
ENDIF
IF A40COL
  ld b,6
ENDIF
IF A36COL
  ld b,7
ENDIF
IF A32COL
  ld b,8
ENDIF
IF A28COL
  ld b,8
ENDIF
IF A24COL
  ld b,9
ENDIF

.L2
  rla
  rl (ix+1)
  rl (ix+0)
  djnz L2
.POST
  ld b,6
  inc b
  dec b
  jr z,NEXT
.L3
  rl (ix+1)
  rl (ix+0)
  djnz L3
.NEXT
  add ix,de
  inc hl
  dec c
  jr nz,PRE
  ret


; The font
; 9 dots: MAX 28 columns
; 8 dots: MAX 32 columns The only one perfecly color aligned
; 7 dots: MAX 36 columns
; 6 dots: MAX 42 columns Good matching with color attributes
; 5 dots: MAX 51 columns
; 4 dots: MAX 64 columns Matched with color attributes (2 by 2)
; 3 dots: MAX 85 columns Just readable!
; 2 dots: MAX 128 columns (useful for ANSI graphics only.. maybe)
; Address 15360 for ROM Font

.font
IF ROMFONT
	; nothing here !
ELSE
	IF PACKEDFONT
	        BINARY  "stdio/ansi/F4PACK.BIN"
	ELSE
		IF A128COL
			BINARY  "stdio/ansi/F3.BIN"
		ENDIF
		IF A80COL
			BINARY  "stdio/ansi/F4.BIN"
		ENDIF
		IF A85COL
			BINARY  "stdio/ansi/F4.BIN"
		ENDIF
		IF A64COL
			BINARY  "stdio/ansi/F4.BIN"
		ENDIF
		IF A51COL
			BINARY  "stdio/ansi/F5.BIN"
		ENDIF
		IF A42COL
			BINARY  "stdio/ansi/F6.BIN"
		ENDIF
		IF A40COL
			BINARY  "stdio/ansi/F6.BIN"
		ENDIF
		IF A36COL
			BINARY  "stdio/ansi/F8.BIN"
		ENDIF
		IF A32COL
			BINARY  "stdio/ansi/F8.BIN"
		ENDIF
		IF A28COL
			BINARY  "stdio/ansi/F8.BIN"
		ENDIF
		IF A24COL
			BINARY  "stdio/ansi/F8.BIN"
		ENDIF
	ENDIF
ENDIF
