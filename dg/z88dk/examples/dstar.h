#pragma string name DStar
#pragma output nostreams

void MovePiece(char *ptr, char plusx, char plusy);
char TestNextPosIsStop(char nextpos);
char CheckNotFinished(void);
void SetupLevel(void);
void DrawBoard(void);
void Gamekeys(void);

char BallOffset;        /* Ball position */
char BoxOffset;         /* Box position */
char PieceIsBall;       /* 1 = box, 0 = ball */
char Level;             /* Guess! */
char Board[144];        /* Space for decompressed Level */

#define MAXLEVEL 25     /* Highest Level   */
#define STARTLEV  1     /* Start Level */
#define TRUE      1
#define FALSE     0

/* Block numbers.. */
#define WALL 1
#define BUBB 2
#define BALL 3
#define BOX  4


#define K_NEXTLEV '+'
#define K_PREVLEV '-'


#if defined __TI82__ || defined __TI83__ || defined __TI8X__ || defined __TI85__ || defined __TI86__
#define K_UP      11  /* arrow up     */
#define K_DOWN    10  /* arrow down   */
#define K_LEFT     8  /* arrow left   */
#define K_RIGHT    9  /* arrow right  */
#define K_SWITCH  13  /* [Enter]      */
#define K_EXIT    '7' /* [Esc]/[Quit] */
#define K_CLEAR   '9'
#endif

#if __MC1000__ || defined  __VZ200__ || defined __NASCOM__ || defined __TRS80__ || defined __GAL__ || defined __MTX__
#define K_UP       'Q'  /* arrow up     */
#define K_DOWN     'A' /* arrow down   */
#define K_LEFT     'O'  /* arrow left   */
#define K_RIGHT    'P'  /* arrow right  */
#define K_SWITCH   ' '  /* [SPACE]      */
#define K_EXIT     'G' /* [Esc]/[Quit] */
#define K_CLEAR    'H'
#endif

//#if defined __SPECTRUM__ || defined __Z88__ || defined __ACE__ || defined __CPC__
#ifndef K_UP
#define K_UP       'q'  /* arrow up     */
#define K_DOWN     'a' /* arrow down   */
#define K_LEFT     'o'  /* arrow left   */
#define K_RIGHT    'p'  /* arrow right  */
#define K_SWITCH   ' '  /* [SPACE]      */
#define K_EXIT     'g' /* [Esc]/[Quit] */
#define K_CLEAR    'h'
#endif

extern char levels[];
extern char sprites[];

#if (spritesize == 4)
#asm
._sprites
 defb    4,4
 defb    @00000000 ; empty sprite
 defb    @00000000
 defb    @00000000
 defb    @00000000
 
 defb    4,4
 defb    @11100000	;1=edge,
 defb    @11100000
 defb    @11100000
 defb    @00000000
 
 defb    4,4
 defb    @01000000	;2=bubble
 defb    @10100000
 defb    @01000000
 defb    @00000000
 
 defb    4,4
 defb    @01000000	;3=moveable ball
 defb    @11100000
 defb    @01000000
 defb    @00000000

 defb    4,4
 defb    @11100000	;4=moveable block
 defb    @10100000
 defb    @11100000
 defb    @00000000
#endasm
#endif

#if (spritesize == 5)
#asm
._sprites
 defb    5,5
 defb    @00000000 ; empty sprite
 defb    @00000000
 defb    @00000000
 defb    @00000000
 defb    @00000000

 defb    5,5
 defb    @11110000	;1=edge,
 defb    @10110000
 defb    @11010000
 defb    @11110000
 defb    @00000000

 defb    5,5
 defb    @01100000	;2=bubble
 defb    @10010000
 defb    @10010000
 defb    @01100000
 defb    @00000000

 defb    5,5
 defb    @01100000	;3=moveable ball
 defb    @11010000
 defb    @11110000
 defb    @01100000
 defb    @00000000

 defb    5,5
 defb    @11110000	;4=moveable block
 defb    @10010000
 defb    @10010000
 defb    @11110000
 defb    @00000000
#endasm
#endif

#if (spritesize == 6)
#asm
._sprites
 defb    6,6
 defb    @00000000 ; empty sprite
 defb    @00000000
 defb    @00000000
 defb    @00000000
 defb    @00000000
 defb    @00000000

 defb    6,6
 defb    @01111000	;1=edge,
 defb    @10000100
 defb    @10000100
 defb    @10000100
 defb    @10000100
 defb    @01111000

 defb    6,6
 defb    @00000000	;2=bubble
 defb    @00110000
 defb    @01001000
 defb    @01001000
 defb    @00110000
 defb    @00000000

 defb    6,6
 defb    @00000000	;3=moveable ball
 defb    @00110000
 defb    @01101000
 defb    @01111000
 defb    @00110000
 defb    @00000000

 defb    6,6
 defb    @00000000	;4=moveable block
 defb    @01111000
 defb    @01001000
 defb    @01001000
 defb    @01111000
 defb    @00000000
#endasm
#endif


#if (spritesize == 7)
#asm
._sprites
                        
 defb	7,7	;0=blank
 defb    @00000000
 defb    @00000000
 defb    @00000000
 defb    @00000000
 defb    @00000000
 defb    @00000000
 defb    @00000000

defb	8,7	;1=edge,
 defb    @01111110
 defb    @10101001
 defb    @11000111
 defb    @10110001
 defb    @11001011
 defb    @10100101
 defb    @01111110

defb	7,7	;2=clear ball
 defb    @00000000
 defb    @00011000
 defb    @00100100
 defb    @00100100
 defb    @00011000
 defb    @00000000
 defb    @00000000

defb	7,7	;3=moveable ball
 defb    @00000000
 defb    @00011000
 defb    @00110100
 defb    @00111100
 defb    @00011000
 defb    @00000000
 defb    @00000000

defb	7,7	;4=moveable block
 defb    @00000000
 defb    @00111100
 defb    @00111100
 defb    @00111100
 defb    @00111100
 defb    @00000000
 defb    @00000000

#endasm
#endif

#if (spritesize == 8)
#asm
._sprites
 defb    8,8
 defb    @00000000	; empty sprite
 defb    @00000000
 defb    @00000000
 defb    @00000000
 defb    @00000000
 defb    @00000000
 defb    @00000000
 defb    @00000000

 defb    8,8
 defb    @01111110	;1=edge,
 defb    @10101001
 defb    @11000111
 defb    @10110001
 defb    @11001011
 defb    @10100101
 defb    @10101001
 defb    @01111110

 defb    8,8
 defb    @00000000	;2=bubble
 defb    @00000000
 defb    @00011000
 defb    @00100100
 defb    @00100100
 defb    @00011000
 defb    @00000000
 defb    @00000000

 defb    8,8
 defb    @00000000	;3=moveable ball
 defb    @00111100
 defb    @01110110
 defb    @01111010
 defb    @01111010
 defb    @01111110
 defb    @00111100
 defb    @00000000

 defb    8,8
 defb    @00000000	;4=moveable block
 defb    @01111110
 defb    @01000010
 defb    @01000010
 defb    @01000010
 defb    @01000010
 defb    @01111110
 defb    @00000000
#endasm
#endif

#if ((spritesize == 15)|(spritesize == 16))
#asm
._sprites
 defb    16,16
 defw    0,0,0,0,0,0,0,0	; empty sprite
 defw    0,0,0,0,0,0,0,0
 
 defb    16,16
 defb    @01111111, @11111110	;1=edge,
 defb    @10101010, @10101001
 defb    @11010101, @01000001
 defb    @10101000, @00000001
 defb    @11010000, @00000001
 defb    @10100000, @00000001
 defb    @11000000, @00000001
 defb    @10000000, @00000001
 defb    @10000000, @00000001
 defb    @10000000, @00000001
 defb    @10000000, @00000001
 defb    @10000000, @00000001
 defb    @10000000, @00000001
 defb    @10000000, @00000001
 defb    @10000000, @00000001
 defb    @01111111, @11111110

 defb    16,16
 defb    @00000000, @00000000	;2=bubble,
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000011, @11000000
 defb    @00000100, @00100000
 defb    @00001000, @10010000
 defb    @00001000, @01010000
 defb    @00001000, @00010000
 defb    @00001000, @00010000
 defb    @00000100, @00100000
 defb    @00000011, @11000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000

 defb    16,16
 defb    @00000000, @00000000	;3=moveable ball
 defb    @00000000, @00000000
 defb    @00000011, @11000000
 defb    @00001111, @00110000
 defb    @00011111, @11011000
 defb    @00011111, @11101000
 defb    @00111111, @11101100
 defb    @00111111, @11111100
 defb    @00111111, @11111100
 defb    @00111111, @11111100
 defb    @00011111, @11111000
 defb    @00011111, @11111000
 defb    @00001111, @11110000
 defb    @00000011, @11000000
 defb    @00000000, @00000000
 defb    @00000000, @00000000

 defb    16,16
 defb    @00000000, @00000000	;4=moveable block
 defb    @01111111, @11111110
 defb    @01001101, @10110010
 defb    @01011111, @11111010
 defb    @01110000, @00001110
 defb    @01110000, @00001110
 defb    @01010000, @00001010
 defb    @01110000, @00001110
 defb    @01110000, @00001110
 defb    @01010000, @00001010
 defb    @01110000, @00001110
 defb    @01110000, @00001110
 defb    @01011111, @11111010
 defb    @01001101, @10110010
 defb    @01111111, @11111110
 defb    @00000000, @00000000
#endasm
#endif


#if (spritesize == 21)
// Generated by Daniel McKinnon's z88dk Sprite Editor
char sprites[] = {
	 32, 22, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 32, 22, 0x3F , 0xFF , 0xFF , 0xFC , 0x55 , 0x55 , 0x40 , 0x02 , 0xAA , 0xAA , 0x00 
, 0x01 , 0xD5 , 0x50 , 0x00 , 0x01 , 0xAA , 0x80 , 0x00 , 0x01 , 0xD4 , 0x00 
, 0x00 , 0x01 , 0xA8 , 0x00 , 0x00 , 0x01 , 0xD4 , 0x00 , 0x00 , 0x01 , 0xA8 
, 0x00 , 0x00 , 0x01 , 0xD0 , 0x00 , 0x00 , 0x01 , 0xA0 , 0x00 , 0x00 , 0x03 
, 0xD0 , 0x00 , 0x00 , 0x01 , 0xA0 , 0x00 , 0x00 , 0x03 , 0xC0 , 0x00 , 0x00 
, 0x01 , 0xA0 , 0x00 , 0x00 , 0x03 , 0xC0 , 0x00 , 0x00 , 0x05 , 0x80 , 0x00 
, 0x00 , 0x03 , 0xC0 , 0x00 , 0x00 , 0x05 , 0x80 , 0x00 , 0x00 , 0x0B , 0x80 
, 0x00 , 0x00 , 0x15 , 0x40 , 0x00 , 0x00 , 0xAA , 0x3F , 0xFF , 0xFF , 0xFC 
, 32, 22, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x07 , 0xE0 , 0x00 , 0x00 , 0x18 
, 0x18 , 0x00 , 0x00 , 0x20 , 0x04 , 0x00 , 0x00 , 0x40 , 0x62 , 0x00 , 0x00 
, 0x80 , 0x11 , 0x00 , 0x00 , 0x80 , 0x11 , 0x00 , 0x00 , 0x80 , 0x01 , 0x00 
, 0x00 , 0x80 , 0x01 , 0x00 , 0x00 , 0x40 , 0x02 , 0x00 , 0x00 , 0x20 , 0x04 
, 0x00 , 0x00 , 0x18 , 0x18 , 0x00 , 0x00 , 0x07 , 0xE0 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 32, 22, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x0F , 0xF0 , 0x00 , 0x00 , 0x3F , 0xFC , 0x00 , 0x00 , 0xFF 
, 0xDF , 0x00 , 0x01 , 0xFF , 0xE7 , 0x80 , 0x03 , 0xFF , 0xF3 , 0xC0 , 0x03 
, 0xFF , 0xF9 , 0xC0 , 0x07 , 0xFF , 0xF9 , 0xE0 , 0x07 , 0xFF , 0xF9 , 0xE0 
, 0x07 , 0xFF , 0xFD , 0xE0 , 0x07 , 0xFF , 0xFF , 0xE0 , 0x03 , 0xFF , 0xFF 
, 0xC0 , 0x03 , 0xFF , 0xFF , 0xC0 , 0x01 , 0xFF , 0xFF , 0x80 , 0x00 , 0xFF 
, 0xFF , 0x00 , 0x00 , 0x3F , 0xFC , 0x00 , 0x00 , 0x0F , 0xF0 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 32, 22, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x3F , 0xFF , 0xFF 
, 0xFC , 0x35 , 0x55 , 0x55 , 0x5C , 0x3A , 0xAA , 0xAA , 0xAC , 0x34 , 0x00 
, 0x00 , 0x5C , 0x3A , 0x00 , 0x00 , 0x2C , 0x34 , 0x00 , 0x00 , 0x5C , 0x3A 
, 0x00 , 0x00 , 0x2C , 0x34 , 0x00 , 0x00 , 0x5C , 0x3A , 0x00 , 0x00 , 0x2C 
, 0x34 , 0x00 , 0x00 , 0x5C , 0x3A , 0x00 , 0x00 , 0x2C , 0x34 , 0x00 , 0x00 
, 0x5C , 0x3A , 0x00 , 0x00 , 0x2C , 0x34 , 0x00 , 0x00 , 0x5C , 0x3A , 0x00 
, 0x00 , 0x2C , 0x35 , 0x55 , 0x55 , 0x5C , 0x3A , 0xAA , 0xAA , 0xAC , 0x3F 
, 0xFF , 0xFF , 0xFC , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
 };

#endif


#if (spritesize == 28)
char sprites[] = { 64, 28, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00
, 64, 28, 0x0F , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xF0 , 0x35 , 0x55 , 0x55 
, 0x55 , 0x50 , 0x00 , 0x00 , 0x1C , 0x6A , 0xAA , 0xAA , 0xA8 , 0x00 , 0x00 
, 0x00 , 0x06 , 0xD5 , 0x55 , 0x55 , 0x00 , 0x00 , 0x00 , 0x00 , 0x03 , 0xEA 
, 0xAA , 0x80 , 0x00 , 0x00 , 0x00 , 0x00 , 0x03 , 0xD5 , 0x50 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x03 , 0xEA , 0xA8 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x03 , 0xD5 , 0x40 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x03 , 0xEA , 0xA0 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x03 , 0xD5 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x03 , 0xEA , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x03 
, 0xD5 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x07 , 0xEA , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x0B , 0xD4 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x07 , 0xE8 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x0B , 0xD4 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x07 , 0xE0 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x0B , 0xD0 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x17 , 0xE8 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x2B , 0xC0 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x17 , 0xC0 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x2B , 0xC0 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 , 0x57 
, 0xC0 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x02 , 0xAB , 0xC0 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x01 , 0x55 , 0x57 , 0x40 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0xAA , 0xAA , 0x60 , 0x00 , 0x00 , 0x00 , 0x00 , 0x15 , 0x55 , 0x56 , 0x38 
, 0x00 , 0x00 , 0x00 , 0x00 , 0xAA , 0xAA , 0xBC , 0x0F , 0xFF , 0xFF , 0xFF 
, 0xFF , 0xFF , 0xFF , 0xF0
, 64, 28, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0xFF , 0xFF , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x0F , 0x80 
, 0x01 , 0xF0 , 0x00 , 0x00 , 0x00 , 0x00 , 0x3C , 0x00 , 0x00 , 0x3C , 0x00 
, 0x00 , 0x00 , 0x00 , 0x70 , 0x00 , 0x38 , 0x0E , 0x00 , 0x00 , 0x00 , 0x00 
, 0xE0 , 0x00 , 0x0E , 0x07 , 0x00 , 0x00 , 0x00 , 0x01 , 0xC0 , 0x00 , 0x03 
, 0x07 , 0x80 , 0x00 , 0x00 , 0x01 , 0xC0 , 0x00 , 0x03 , 0x03 , 0x80 , 0x00 
, 0x00 , 0x03 , 0xC0 , 0x00 , 0x00 , 0x03 , 0xC0 , 0x00 , 0x00 , 0x03 , 0x80 
, 0x00 , 0x00 , 0x01 , 0xC0 , 0x00 , 0x00 , 0x03 , 0x80 , 0x00 , 0x00 , 0x01 
, 0xC0 , 0x00 , 0x00 , 0x03 , 0x80 , 0x00 , 0x00 , 0x01 , 0xC0 , 0x00 , 0x00 
, 0x03 , 0xC0 , 0x00 , 0x00 , 0x03 , 0xC0 , 0x00 , 0x00 , 0x01 , 0xC0 , 0x00 
, 0x00 , 0x03 , 0x80 , 0x00 , 0x00 , 0x01 , 0xC0 , 0x00 , 0x00 , 0x07 , 0x80 
, 0x00 , 0x00 , 0x00 , 0xE0 , 0x00 , 0x00 , 0x07 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x78 , 0x00 , 0x00 , 0x0E , 0x00 , 0x00 , 0x00 , 0x00 , 0x3C , 0x00 , 0x00 
, 0x3C , 0x00 , 0x00 , 0x00 , 0x00 , 0x0F , 0x80 , 0x01 , 0xF0 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0xFF , 0xFF , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00
, 64, 28, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x07 
, 0xFF , 0xFF , 0xE0 , 0x00 , 0x00 , 0x00 , 0x00 , 0xFF , 0xFF , 0xFF , 0xFF 
, 0x00 , 0x00 , 0x00 , 0x07 , 0xFF , 0xFF , 0xFF , 0xFF , 0xE0 , 0x00 , 0x00 
, 0x1F , 0xFF , 0xFF , 0xFF , 0xFF , 0xF8 , 0x00 , 0x00 , 0x7F , 0xFF , 0xFF 
, 0xFE , 0x3F , 0xFE , 0x00 , 0x00 , 0xFF , 0xFF , 0xFF , 0xFF , 0xCF , 0xFF 
, 0x00 , 0x01 , 0xFF , 0xFF , 0xFF , 0xFF , 0xF3 , 0xFF , 0x80 , 0x01 , 0xFF 
, 0xFF , 0xFF , 0xFF , 0xF8 , 0xFF , 0x80 , 0x03 , 0xFF , 0xFF , 0xFF , 0xFF 
, 0xFC , 0x7F , 0xC0 , 0x03 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFC , 0x3F , 0xC0 
, 0x07 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFE , 0x3F , 0xE0 , 0x07 , 0xFF , 0xFF 
, 0xFF , 0xFF , 0xFE , 0x3F , 0xE0 , 0x07 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
, 0x7F , 0xE0 , 0x07 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xE0 , 0x07 
, 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xE0 , 0x07 , 0xFF , 0xFF , 0xFF 
, 0xFF , 0xFF , 0xFF , 0xE0 , 0x03 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF 
, 0xC0 , 0x03 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xC0 , 0x01 , 0xFF 
, 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0x80 , 0x01 , 0xFF , 0xFF , 0xFF , 0xFF 
, 0xFF , 0xFF , 0x80 , 0x00 , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0x00 
, 0x00 , 0x7F , 0xFF , 0xFF , 0xFF , 0xFF , 0xFE , 0x00 , 0x00 , 0x1F , 0xFF 
, 0xFF , 0xFF , 0xFF , 0xF8 , 0x00 , 0x00 , 0x07 , 0xFF , 0xFF , 0xFF , 0xFF 
, 0xE0 , 0x00 , 0x00 , 0x00 , 0xFF , 0xFF , 0xFF , 0xFF , 0x00 , 0x00 , 0x00 
, 0x00 , 0x07 , 0xFF , 0xFF , 0xE0 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00
, 64, 28, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x0F , 0xFF , 0xFF 
, 0xFF , 0xFF , 0xFF , 0xFF , 0xF0 , 0x0F , 0x73 , 0x33 , 0x33 , 0x33 , 0x33 
, 0x33 , 0xB0 , 0x0D , 0xCC , 0xCC , 0xCC , 0xCC , 0xCC , 0xCE , 0xF0 , 0x0F 
, 0x73 , 0x33 , 0x33 , 0x33 , 0x33 , 0x33 , 0xB0 , 0x0D , 0xC8 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x0E , 0xF0 , 0x0F , 0x70 , 0x00 , 0x00 , 0x00 , 0x00 , 0x13 
, 0xB0 , 0x0D , 0xC8 , 0x00 , 0x00 , 0x00 , 0x00 , 0x0E , 0xF0 , 0x0F , 0x70 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x13 , 0xB0 , 0x0D , 0xC8 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x0E , 0xF0 , 0x0F , 0x70 , 0x00 , 0x00 , 0x00 , 0x00 , 0x13 , 0xB0 
, 0x0D , 0xC8 , 0x00 , 0x00 , 0x00 , 0x00 , 0x0E , 0xF0 , 0x0F , 0x70 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x13 , 0xB0 , 0x0D , 0xC8 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x0E , 0xF0 , 0x0F , 0x70 , 0x00 , 0x00 , 0x00 , 0x00 , 0x13 , 0xB0 , 0x0D 
, 0xC8 , 0x00 , 0x00 , 0x00 , 0x00 , 0x0E , 0xF0 , 0x0F , 0x70 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x13 , 0xB0 , 0x0D , 0xC8 , 0x00 , 0x00 , 0x00 , 0x00 , 0x0E 
, 0xF0 , 0x0F , 0x70 , 0x00 , 0x00 , 0x00 , 0x00 , 0x13 , 0xB0 , 0x0D , 0xC8 
, 0x00 , 0x00 , 0x00 , 0x00 , 0x0E , 0xF0 , 0x0F , 0x70 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x13 , 0xB0 , 0x0D , 0xC8 , 0x00 , 0x00 , 0x00 , 0x00 , 0x0E , 0xF0 
, 0x0F , 0x70 , 0x00 , 0x00 , 0x00 , 0x00 , 0x13 , 0xB0 , 0x0D , 0xCC , 0xCC 
, 0xCC , 0xCC , 0xCC , 0xCE , 0xF0 , 0x0F , 0x73 , 0x33 , 0x33 , 0x33 , 0x33 
, 0x33 , 0xB0 , 0x0D , 0xCC , 0xCC , 0xCC , 0xCC , 0xCC , 0xCE , 0xF0 , 0x0F 
, 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xF0 , 0x00 , 0x00 , 0x00 , 0x00 
, 0x00 , 0x00 , 0x00 , 0x00  };


#endif


#asm
._levels
 defb    17,30    ;ball offset, box offset
 defb    @01010101,@01010101,@01010101,@01010101
 defb    @01000101,@00000000,@00000000,@10010001
 defb    @01000000,@00000000,@00000010,@00010101
 defb    @01000000,@00000000,@01011000,@00000001
 defb    @01000000,@01010010,@00000000,@00000101
 defb    @01010010,@00001000,@00000000,@10000001
 defb    @01001000,@00000000,@00100101,@00100001
 defb    @01000000,@00000101,@10000000,@00001001
 defb    @01010101,@01010101,@01010101,@01010101
.level2
 defb    30,86
 defb    @00010000,@01000100,@01000000,@01000101
 defb    @01000000,@10000000,@00000000,@00000001
 defb    @00000001,@10000001,@10000000,@10000000
 defb    @01000100,@10000000,@00001000,@00010001
 defb    @00000000,@00000100,@00001000,@00000100
 defb    @01000000,@00010001,@00001000,@00000001
 defb    @00000001,@00000100,@01000000,@01101001
 defb    @01000000,@00000000,@00000000,@00000100
 defb    @00010000,@01000000,@00000000,@00010000
.level3
 defb    30,46
 defb    @01010101,@01010101,@01010101,@01010101
 defb    @01000000,@00000000,@00000000,@10010001
 defb    @01000000,@01010000,@00000000,@01010001
 defb    @01000000,@01100000,@00000010,@00000001
 defb    @01001000,@00000000,@10010100,@00001001
 defb    @01000110,@00001000,@00100100,@00100101
 defb    @01000101,@10000110,@00001000,@10010101
 defb    @01100000,@00000101,@10000000,@00000001
 defb    @01010101,@01010101,@01010101,@01010101
.level4
 defb    125,30
 defb    @01010101,@01010101,@01010101,@01010101
 defb    @01000000,@00000000,@00000000,@00010001
 defb    @01000000,@00000100,@00000000,@00000001
 defb    @01011001,@10001001,@10011001,@10011001
 defb    @01000100,@01100010,@01000100,@01000101
 defb    @01011001,@10011000,@10011001,@10011001
 defb    @01000000,@00000100,@00000000,@00000001
 defb    @01000000,@01000000,@00000000,@01000001
 defb    @01010101,@01010101,@01010101,@01010101
.level5
 defb    17,110
 defb    @00010101,@01010101,@01010101,@01010100
 defb    @01000000,@01000000,@01000001,@00000001
 defb    @01000001,@10000100,@10000010,@00010001
 defb    @01010000,@00000000,@01000001,@00000001
 defb    @01100001,@10010000,@00000000,@00000101
 defb    @01010000,@00000001,@00100001,@00000001
 defb    @01100100,@00010001,@00010000,@00010001
 defb    @01000000,@01000000,@00100100,@00011001
 defb    @00010101,@01010101,@01010101,@01010100
.level6
 defb    65,113
 defb    @00000000,@01010101,@01010101,@01010101
 defb    @00000001,@00000010,@00000001,@10001001
 defb    @00000100,@00000010,@00000000,@01000101
 defb    @00010000,@00000010,@00000000,@00000001
 defb    @01000000,@00000010,@00000000,@00000001
 defb    @01010000,@00000010,@00000100,@00000101
 defb    @01000000,@00000010,@00000000,@01000001
 defb    @01000001,@00000010,@00000101,@10000001
 defb    @01010101,@01010101,@01010101,@01010101
.level7
 defb    115,122
 defb    @01010101,@01010101,@01010101,@01010101
 defb    @01000000,@00000000,@00000000,@00000001
 defb    @00010100,@01010100,@00011000,@01011001
 defb    @00011000,@00011000,@01000100,@01000100
 defb    @00000100,@00010000,@01010100,@01010000
 defb    @00010100,@00010000,@01100100,@01100100
 defb    @01000000,@00000000,@00000000,@00000001
 defb    @01000000,@01100000,@00000000,@00011001
 defb    @01010101,@01010101,@01010101,@01010101
.level8
 defb    108,98
 defb    @01010101,@01010101,@01010101,@01010100
 defb    @01000010,@01010000,@00000000,@00000101
 defb    @01000001,@10000001,@01001000,@00000001
 defb    @01000010,@01010001,@00011000,@00000001
 defb    @01010000,@00000001,@01000001,@10010001
 defb    @01010001,@00000000,@00000010,@01100001
 defb    @01100010,@01000000,@10000001,@00010001
 defb    @01010000,@00000000,@00000000,@00000001
 defb    @00010101,@01010101,@01010101,@01010101
.level9
 defb    30,72
 defb    @00000100,@01010101,@01010101,@01010100
 defb    @00011001,@10000000,@00000001,@00000001
 defb    @01100010,@01000000,@00100000,@00000100
 defb    @00010001,@00001001,@01000010,@01000001
 defb    @01000001,@10000110,@00100000,@00001001
 defb    @01000000,@00001001,@01000000,@00000100
 defb    @01100110,@00000000,@00000000,@00010000
 defb    @01000000,@00000000,@00000000,@01000000
 defb    @01010101,@01010101,@01010101,@00000000
.level10
 defb    93,36
 defb    @00000000,@01010101,@01010101,@01010100
 defb    @01010101,@00100000,@00000000,@00000001
 defb    @01000000,@00000101,@01100010,@01001001
 defb    @01001000,@00000110,@00011000,@00000100
 defb    @01000000,@00000100,@00100000,@01001001
 defb    @01100110,@00000100,@10010000,@01000100
 defb    @00011000,@00000101,@01000001,@01010000
 defb    @01000000,@00000000,@00000100,@01000100
 defb    @00010101,@01010101,@01010000,@01000001
.level11
 defb    30,108
 defb    @01010101,@01010101,@01010101,@01010101
 defb    @01000000,@00000001,@00000000,@00000001
 defb    @01000001,@10100000,@00000010,@10000101
 defb    @01010000,@00100000,@00010100,@00001001
 defb    @01100000,@00000110,@01101000,@00010101
 defb    @01010001,@01000000,@00010100,@00000001
 defb    @01100000,@10010010,@00000000,@00001001
 defb    @01011001,@01010000,@00000100,@00000101
 defb    @00010100,@01010101,@01010101,@01010100
.level12
 defb    17,92
 defb    @01010000,@00000001,@01000001,@01010100
 defb    @01000101,@01010110,@00010101,@00100101
 defb    @01000000,@00101000,@00000000,@10000001
 defb    @01000101,@00000101,@10000001,@10010001
 defb    @01000100,@10000101,@01100001,@01000001
 defb    @01000101,@00000101,@00000001,@00010001
 defb    @01000000,@00001000,@00000000,@00000001
 defb    @01000000,@00000000,@00100000,@00000001
 defb    @01010101,@01010101,@01010101,@01010101
.level13
 defb    18,113
 defb    @00010101,@01010101,@01010101,@01010100
 defb    @01000001,@00000000,@00000000,@10000101
 defb    @01000100,@00000110,@00000010,@01010001
 defb    @01000000,@00000000,@10000000,@00010001
 defb    @01001000,@00000000,@00000000,@00011001
 defb    @01000100,@00000000,@00100000,@00000001
 defb    @01010000,@00000000,@10001000,@00011001
 defb    @01000000,@01000000,@00100001,@00010001
 defb    @00010101,@01010101,@01010101,@01010100
.level14
 defb    36,50
 defb    @01010101,@01010101,@01010101,@01010101
 defb    @01100110,@00000000,@00000000,@10011001
 defb    @01001001,@00000000,@00000001,@01000001
 defb    @01000000,@00000000,@00000010,@00000001
 defb    @01000000,@00000000,@00100100,@00000001
 defb    @01000000,@00000010,@00000000,@00000001
 defb    @01001001,@00000000,@00000000,@01000001
 defb    @01100110,@00000000,@00000000,@10011001
 defb    @01010101,@01010101,@01010101,@01010101
.level15
 defb    51,76
 defb    @00010101,@01010100,@01010101,@01010100
 defb    @01000000,@00001001,@00000000,@00100001
 defb    @01000100,@10000100,@00010000,@00100001
 defb    @01000000,@01000000,@01101000,@01100001
 defb    @00010001,@00000001,@00100000,@00010001
 defb    @01100000,@00000000,@00010000,@01100001
 defb    @00010000,@00000000,@10000000,@00000100
 defb    @01100000,@00000000,@00000000,@00001001
 defb    @00010101,@01010101,@01010101,@01010100
.level16
 defb    35,19
 defb    @01010101,@01010101,@01010101,@01010101
 defb    @01010000,@01100010,@00000000,@00001001
 defb    @01100000,@10011000,@00000000,@00000101
 defb    @01010001,@01010000,@00001000,@00000101
 defb    @01010000,@00000010,@01100100,@00000001
 defb    @01101000,@00000000,@00001001,@10000001
 defb    @01010010,@00000000,@01010101,@10000001
 defb    @01011001,@00000100,@00000000,@00000001
 defb    @01010101,@01010101,@01010101,@01010101
.level17
 defb    29,124
 defb    @01010101,@01010101,@01010101,@01010101
 defb    @01001001,@00000000,@00000000,@01000001
 defb    @01000100,@00100110,@10011000,@00010001
 defb    @01000000,@00011001,@01100100,@10000001
 defb    @01001001,@00000000,@00000010,@01000001
 defb    @01000010,@01100000,@00001001,@00000001
 defb    @01000100,@00010001,@01100100,@00010001
 defb    @01000000,@00100001,@10000000,@00000001
 defb    @01010101,@01010101,@01010101,@01010101
.level18
 defb    115,26
 defb    @01010101,@01010101,@01010101,@01010101
 defb    @01001000,@00000010,@00000001,@00000001
 defb    @01000001,@10011000,@00000110,@00000001
 defb    @01000000,@01100100,@00000001,@10000001
 defb    @01000000,@10000001,@00000010,@01100001
 defb    @01000110,@01000000,@01001001,@00000001
 defb    @01001001,@10000100,@10000100,@00000001
 defb    @01100100,@00000100,@00000000,@01000001
 defb    @01010101,@01010101,@01010101,@01010101
.level19
 defb    126,110
 defb    @01010101,@01010101,@01010101,@01010101
 defb    @01100000,@00010100,@00000000,@01011001
 defb    @01000100,@00010000,@00000000,@01100001
 defb    @01001001,@00000010,@01010000,@10000001
 defb    @01000100,@00000001,@10000000,@00000001
 defb    @01000000,@00010000,@00100100,@00000001
 defb    @01000101,@00100100,@01011000,@00010001
 defb    @01001001,@00011000,@00000000,@01010001
 defb    @01010101,@01010101,@01010101,@01010101
.level20
 defb    77,66
 defb    @01010101,@01010101,@01010101,@01010101
 defb    @01000000,@10011000,@00000000,@00000001
 defb    @01011000,@00100100,@01011000,@00000101
 defb    @01000100,@01001000,@00000100,@00010001
 defb    @01000000,@01000001,@01000001,@00001001
 defb    @01000100,@00010000,@00100001,@00010001
 defb    @01010000,@00100101,@00011000,@00100101
 defb    @01000000,@00000000,@00100110,@00000001
 defb    @01010101,@01010101,@01010101,@01010101
.level21
 defb    103,105
 defb    @01010101,@01010101,@01010101,@01010101
 defb    @01000101,@01000000,@00001000,@00000101
 defb    @01000000,@01000000,@00000000,@01000101
 defb    @01000000,@01011000,@00000000,@00100001
 defb    @01000010,@00000000,@10000000,@10000101
 defb    @01000000,@00010000,@00000101,@01100001
 defb    @01000010,@00100000,@00000010,@00101001
 defb    @01010101,@01010101,@01010101,@01010101
 defb    @00000000,@00000000,@00000000,@00000000
.level22
 defb    103,105
 defb    @01010101,@01010101,@01010101,@01010101
 defb    @01100100,@00011001,@00011000,@00010001
 defb    @01000000,@00010000,@00000000,@00000001
 defb    @01100000,@00010000,@01100000,@10000001
 defb    @01010001,@10000000,@00000010,@00010101
 defb    @01001000,@01000000,@01010110,@00000001
 defb    @01000000,@00000100,@01000000,@10000001
 defb    @01010101,@01010101,@01010101,@01010101
 defb    @00000000,@00000000,@00000000,@00000000
.level23
 defb    103,105
 defb    @00010101,@01010101,@01010101,@01010100
 defb    @01000100,@00011001,@00011000,@00010001
 defb    @01000000,@00100000,@01000000,@00000001
 defb    @01010000,@00010000,@00100001,@10000001
 defb    @01000001,@10000001,@00001010,@00100001
 defb    @01011000,@01000000,@01010010,@00000001
 defb    @01000000,@00000100,@01000000,@10000001
 defb    @00010101,@01010101,@01010101,@01010100
 defb    @00000000,@00000000,@00000000,@00000000
.level24
 defb    103,105
 defb    @01010101,@01010101,@01010101,@01010101
 defb    @01000000,@00000100,@00000000,@00000101
 defb    @01000101,@10001000,@00000001,@01100101
 defb    @01000110,@00000000,@00100100,@00010101
 defb    @01001010,@00001001,@00010100,@00000001
 defb    @01000110,@00100001,@00000000,@01010001
 defb    @01000101,@00000000,@01000101,@01101001
 defb    @01010101,@01010101,@01010101,@01010101
 defb    @00000000,@00000000,@00000000,@00000000
.level25
 defb    103,105
 defb    @01010101,@01010101,@01010101,@01010101
 defb    @01000000,@00000000,@00000000,@00010001
 defb    @01001000,@01011000,@00001000,@00000001
 defb    @01000000,@01100000,@10000001,@01000001
 defb    @01001000,@00000001,@01000001,@10000001
 defb    @01000110,@00000010,@01000000,@00100001
 defb    @01000101,@10000100,@00000000,@00000001
 defb    @01010101,@01010101,@01010101,@01010101
 defb    @00000000,@00000000,@00000000,@00000000
#endasm