;
; 	ANSI Video handling for the Epson PX4
;	By Stefano Bodrato - Nov 2014
;
; 	Handles Attributes INVERSE + UNDERLINED
;
;	** alternate (smaller) 4bit font capability: 
;	** use the PACKEDFONT flag
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
;	$Id: f_ansi_char.asm,v 1.1 2015/11/05 16:08:04 stefano Exp $
;

	PUBLIC	ansi_CHAR
	
	EXTERN	ansi_ROW
	EXTERN	ansi_COLUMN
	
	PUBLIC	text_cols
	PUBLIC	text_rows
	
; Dirty thing for self modifying code
	PUBLIC	INVRS

; Setting char_dots to 3 the columns number can be 80 !
	defc char_dots = 4
	defc PACKEDFONT = 1
	
.text_cols   defb 60
.text_rows   defb 8


.ansi_CHAR
  	ld (char+1),a

  	ld  hl,$e000

	ld	a,(ansi_ROW)

	and	a
	jr	z,ZROW
	add	a,a
	add	a,a
	add	a,a
	ld	b,a	; b=a*8  (8 is the font height)
	ld	de,32	; row_bytes
.Rloop
	add	hl,de
	djnz	Rloop
.ZROW

  ld (RIGA+1),hl	; RIGA+1=ROW Location
  ld hl,DOTS+1
  ld b,(hl)		; b=DOTS

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


  ld b,5
.RGTA
  srl a
  djnz RGTA
  
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
.RIGA
  ld de,0	; ROW  Location on screen
  add hl,de
  push hl
  pop ix

.char
  ld b,'A'      ; Put here the character to be printed
  
IF PACKEDFONT
  xor	a
  rr	b
  jr	c,even
  ld	a,4
.even
  ld	(ROLL+1),a
  ld hl,font-128
ELSE
  ld hl,font
ENDIF

  ld de,8

.LFONT
  add hl,de
  djnz LFONT

  ld de,32      ; ROW Lenght (in bytes)
  
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
  
IF PACKEDFONT
.ROLL
  jr INVRS
  rla
  rla
  rla
  rla
ENDIF

; --- --- Inverse text handling
.INVRS
;  cpl		; Set to NOP to disable INVERSE
  nop
; --- ---

; --- --- Underlined text handling
  dec c
;  jr nz,UNDRL	; Set to JR UNDRL to disable underlined text (loc. INVRS+2)
  jr UNDRL
  ld a,255
.UNDRL
  inc c
; --- --- end of underlined text handling

.DOTS
  ld b,char_dots	; character FONT width in pixel
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

.font
        BINARY  "stdio/ansi/F4PACK.BIN"
