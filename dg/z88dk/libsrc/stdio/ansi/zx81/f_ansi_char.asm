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
;	$Id: f_ansi_char.asm,v 1.11 2015/01/23 07:07:31 stefano Exp $
;

	PUBLIC	ansi_CHAR

IF ROMFONT
	LIB	asctozx81
ENDIF
	
	EXTERN	base_graphics

	EXTERN	ansi_ROW
	EXTERN	ansi_COLUMN

	PUBLIC	text_cols
IF G007
	PUBLIC	text_rows
ENDIF
IF MTHRG
	PUBLIC	text_rows
ENDIF
	
; Dirty thing for self modifying code
	PUBLIC	INVRS
	PUBLIC	BOLD

IF A136COL
.text_cols   defb 136
ENDIF

IF A128COL
.text_cols   defb 128
ENDIF

IF A124COL
.text_cols   defb 124
ENDIF

IF A80COL
.text_cols   defb 80
ENDIF

IF A82COL
.text_cols   defb 82
ENDIF

IF A85COL
.text_cols   defb 85
ENDIF

IF A90COL
.text_cols   defb 90
ENDIF

IF A64COL
.text_cols   defb 64
ENDIF

IF A62COL
.text_cols   defb 62
ENDIF

IF A68COL
.text_cols   defb 68
ENDIF

IF A51COL
.text_cols   defb 51
ENDIF

IF A49COL
.text_cols   defb 49
ENDIF

IF A54COL
.text_cols   defb 54
ENDIF

IF A42COL
.text_cols   defb 42
ENDIF

IF A41COL
.text_cols   defb 41
ENDIF

IF A40COL
.text_cols   defb 40
ENDIF

IF A45COL
.text_cols   defb 45
ENDIF

IF A36COL
.text_cols   defb 36
ENDIF

IF B32COL
.text_cols   defb 32
ENDIF

IF A35COL
.text_cols   defb 35
ENDIF

IF A38COL
.text_cols   defb 38
ENDIF

IF A32COL
.text_cols   defb 32
ENDIF

IF A31COL
.text_cols   defb 31
ENDIF

IF A34COL
.text_cols   defb 34
ENDIF

IF A28COL
.text_cols   defb 28
ENDIF

IF A24COL
.text_cols   defb 24
ENDIF

IF A27COL
.text_cols   defb 27
ENDIF

IF G007
.text_rows   defb 23
ENDIF
IF MTHRG
.text_rows   defb 24
ENDIF

.ansi_CHAR
; --- TO USE ROM FONT WE NEED TO MAP TO THE ASCII CODES ---
IF ROMFONT
	ld	hl,char+1
	ld	(hl),a
	call	asctozx81
ENDIF
; --- END OF ROM FONT ADAPTER ---

  ld (char+1),a
  ld a,(ansi_ROW)       ; Line text position
  
IF G007
	ld  h,0
	ld  e,a
	add a
	add a
	add a		; *8
	ld  l,a
	ld  a,e
	add	hl,hl	; *16
	add h
	ld	h,a		; *272
	ld de,(base_graphics)
	add hl,de
	ld de,9
	add hl,de
ELSE
 IF MTHRG
	ld  h,a		; *256
	add a
	add a
	add a		; *8   -> * 264
	ld	l,a

	inc	hl
	inc	hl
	;ld		de,($407B)
	ld de,(base_graphics)
	add hl,de
 ELSE
 ;  ld	d,a
 ;  ld	e,0
  ld hl,(base_graphics)
  add h
  ld  h,a
 ;  add hl,de
 ENDIF
ENDIF
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

  srl h
  rr l
  rra
  srl h
  rr l
  rra
  srl h
  rr l
  rra
IF ARX816
  add hl,hl
  add hl,hl
  add hl,hl
ENDIF

  srl a
  srl a
  srl a
  srl a
  srl a
;  ld b,5
;.RGTA
;  srl a
;  djnz RGTA
.ZCL
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
  ld hl,$1e00
  xor	a
  add	b
  jr	z,NOLFONT
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
.NOLFONT

IF !ARX816
IF G007
  ld de,34	; next row
ELSE
IF MTHRG
  ld de,33	; next row
ELSE
  ld de,32	; next row
ENDIF
ENDIF
ENDIF

  ld c,8
.PRE
  ld b,4
IF ARX816
  rl (ix+8)
ELSE
  rl (ix+1)
ENDIF
  rl (ix+0)
  inc b
  dec b
  jr z,DTS
.L1
IF ARX816
  rl (ix+8)
ELSE
  rl (ix+1)
ENDIF
  rl (ix+0)
  djnz L1
.DTS
  ld a,(hl)

.BOLD
  nop	;	rla
  nop	;	or (hl)
  
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
IF A136COL
  ld b,2
ENDIF
IF A128COL
  ld b,2
ENDIF
IF A124COL
  ld b,2
ENDIF
IF A80COL
  ld b,3
ENDIF
IF A82COL
  ld b,3
ENDIF
IF A85COL
  ld b,3
ENDIF
IF A90COL
  ld b,3
ENDIF
IF A64COL
  ld b,4
ENDIF
IF A62COL
  ld b,4
ENDIF
IF A68COL
  ld b,4
ENDIF
IF A51COL
  ld b,5
ENDIF
IF A49COL
  ld b,5
ENDIF
IF A54COL
  ld b,5
ENDIF
IF A42COL
  ld b,6
ENDIF
IF A41COL
  ld b,6
ENDIF
IF A45COL
  ld b,6
ENDIF
IF A40COL
  ld b,6
ENDIF
IF A36COL
  ld b,7
ENDIF
IF A35COL
  ld b,7
ENDIF
IF B32COL
  ld b,7
ENDIF
IF A38COL
  ld b,7
ENDIF
IF A32COL
  ld b,8
ENDIF
IF A31COL
  ld b,8
ENDIF
IF A34COL
  ld b,8
ENDIF
IF A28COL
  ld b,8
ENDIF
IF A24COL
  ld b,9
ENDIF
IF A27COL
  ld b,9
ENDIF

.L2
  rla
IF ARX816
  rl (ix+8)
ELSE
  rl (ix+1)
ENDIF
  rl (ix+0)
  djnz L2
.POST
  ld b,6
  inc b
  dec b
  jr z,NEXT
.L3
IF ARX816
  rl (ix+8)
ELSE
  rl (ix+1)
ENDIF
  rl (ix+0)
  djnz L3
.NEXT
IF ARX816
  inc ix
ELSE
  add ix,de
ENDIF
  inc hl
  dec c
  jr nz,PRE
  ret


; The font
; 9 dots: MAX 28 columns
; 8 dots: MAX 32 columns
; 7 dots: MAX 36 columns
; 6 dots: MAX 42 columns
; 5 dots: MAX 51 columns
; 4 dots: MAX 64 columns
; 3 dots: MAX 85 columns Just readable!
; 2 dots: MAX 128 columns (useful for ANSI graphics only.. maybe)
; No file for ROM Font

.font
IF ROMFONT
	; nothing here !
ELSE
	IF PACKEDFONT
	        BINARY  "stdio/ansi/F4PACK.BIN"
	ELSE
		IF A136COL
			BINARY  "stdio/ansi/F3.BIN"
		ENDIF
		IF A128COL
			BINARY  "stdio/ansi/F3.BIN"
		ENDIF
		IF A124COL
			BINARY  "stdio/ansi/F3.BIN"
		ENDIF
		IF A80COL
			BINARY  "stdio/ansi/F4.BIN"
		ENDIF
		IF A82COL
			BINARY  "stdio/ansi/F4.BIN"
		ENDIF
		IF A85COL
			BINARY  "stdio/ansi/F4.BIN"
		ENDIF
		IF A90COL
			BINARY  "stdio/ansi/F4.BIN"
		ENDIF
		IF A64COL
			BINARY  "stdio/ansi/F4.BIN"
		ENDIF
		IF A62COL
			BINARY  "stdio/ansi/F4.BIN"
		ENDIF
		IF A68COL
			BINARY  "stdio/ansi/F4.BIN"
		ENDIF
		IF A51COL
			BINARY  "stdio/ansi/F5.BIN"
		ENDIF
		IF A49COL
			BINARY  "stdio/ansi/F5.BIN"
		ENDIF
		IF A54COL
			BINARY  "stdio/ansi/F5.BIN"
		ENDIF
		IF A42COL
			BINARY  "stdio/ansi/F6.BIN"
		ENDIF
		IF A41COL
			BINARY  "stdio/ansi/F6.BIN"
		ENDIF
		IF A45COL
			BINARY  "stdio/ansi/F6.BIN"
		ENDIF
		IF A40COL
			BINARY  "stdio/ansi/F6.BIN"
		ENDIF
		IF A36COL
			BINARY  "stdio/ansi/F8.BIN"
		ENDIF
		IF B32COL
			BINARY  "stdio/ansi/F8.BIN"
		ENDIF
		IF A35COL
			BINARY  "stdio/ansi/F8.BIN"
		ENDIF
		IF A38COL
			BINARY  "stdio/ansi/F8.BIN"
		ENDIF
		IF A32COL
			BINARY  "stdio/ansi/F8.BIN"
		ENDIF
		IF A31COL
			BINARY  "stdio/ansi/F8.BIN"
		ENDIF
		IF A34COL
			BINARY  "stdio/ansi/F8.BIN"
		ENDIF
		IF A28COL
			BINARY  "stdio/ansi/F8.BIN"
		ENDIF
		IF A24COL
			BINARY  "stdio/ansi/F8.BIN"
		ENDIF
		IF A27COL
			BINARY  "stdio/ansi/F8.BIN"
		ENDIF
	ENDIF
ENDIF
