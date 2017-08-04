;
; 	ANSI Video handling for the TI calculators
;	By Stefano Bodrato - Dec. 2000
;
; 	Handles Attributes INVERSE + UNDERLINED
;
;	** alternate (smaller) 4bit font capability: 
;	** use the -DPACKEDFONT flag
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
;	$Id: f_ansi_char.asm,v 1.7 2015/01/19 01:33:19 pauloscustodio Exp $
;

	INCLUDE	"stdio/ansi/ticalc/ticalc.inc"
	
	PUBLIC	ansi_CHAR
	
	EXTERN	ansi_ROW
	EXTERN	ansi_COLUMN
	
	EXTERN	base_graphics
	EXTERN	cpygraph
	
	PUBLIC	text_cols
	PUBLIC	text_rows
	
; Dirty thing for self modifying code
	PUBLIC	INVRS

.text_cols   defb columns
.text_rows   defb 8

.ansi_CHAR
  	ld (char+1),a

  	ld	hl,(base_graphics)

	ld	a,(ansi_ROW)

	and	a
	jr	z,ZROW
	add	a,a
	add	a,a
	add	a,a
	ld	b,a	; b=a*8  (8 is the font height)
	ld	de,row_bytes
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

  ld de,row_bytes      ; ROW Lenght (in bytes)
  
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
  
  jp	cpygraph	; Copy GRAPH_MEM to LCD, then return




; The font

; To keep the same text metrics on both calc 
; families I suggest 32 or 24 columns.
; Here we go with 32...

; TI 82-83
; 6 dots: MAX 16 columns
; 5 dots: MAX 19 columns
; 4 dots: MAX 24 columns
; 3 dots: MAX 32 columns

; TI 85-86
; 6 dots: MAX 21 columns
; 5 dots: MAX 25 columns
; 4 dots: MAX 32 columns
; 3 dots: MAX 42 columns

.font
        BINARY  "stdio/ansi/F4PACK.BIN"
