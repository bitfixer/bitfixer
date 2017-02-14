#include <stdlib.h>
#include "dgprog.h"
//#include <math.h>

#define PROGRAM_LOADED_ADDR     0x0100

#define LINECTR         0x0140
#define TEMP            0x0151
#define SCREENMEM       0x3800

void clear()
{
    unsigned char* lctr;
    __asm
    call #0x00E6
    __endasm;

    // reset line counter
    lctr = (unsigned char*)LINECTR;
    *lctr = 0;
}

void print(unsigned char* str)
{
    str;
    __asm
    loadstack:
        pop	bc
        pop	hl
        push	hl
        push	bc
        ld a, (#LINECTR)
        ld b, a
    printchar:
        ld	a,(hl)
        cp a, #0x00
        jp z,done
        set	7, a
        call #0x00FA
        inc hl
        inc b
        jp printchar
    done:
        ld a, b
        and a, #0x3F        ; mod 64
        ld (#LINECTR), a     ; store line counter value
    __endasm;
}

unsigned char readchar()
{
    unsigned char* temp;
    temp = (unsigned char*)TEMP;
    __asm
    in a, ($00)
    ld (#TEMP), a
    __endasm;

    if ((*temp & 0x80) == 0)
    {
        return 0;
    }

    //return (*temp & 0x7F);
    return *temp;
}

unsigned char readchar_blocking()
{
    unsigned char ch;
    unsigned char ret;
    ch = 0;
    while (ch == 0)
    {
        ch = readchar();
    }

    ret = ch & 0x7F;

    while (ch != 0)
    {
        ch = readchar();
    }

    return ret;
}

// read string from keyboard
// also echo the typed characters to the screen
void readString(unsigned char* str)
{
    unsigned char cc[2];
    unsigned char ch;
    cc[1] = 0;
    ch = readchar_blocking();
    while (ch != 13)
    {
        *str = ch;
        str++;
        // echo the character
        //printchar(ch);
        cc[0] = ch;
        print(cc);
        ch = readchar_blocking();
    }

    *str = 0;
    newline();
}

void newline()
{
    unsigned char* lctr;
    unsigned char sp;
    unsigned char c;

    lctr = (unsigned char*)LINECTR;
    sp = 64 - *lctr;
    for (c = 0; c < sp; c++)
    {
        print(" ");
    }
    *lctr = 0;
}

void printchar(unsigned char c)
{
    c;
    __asm
    ld	iy,#2
    add	iy,sp
    ld	a,0 (iy)
    call #0x00FA

    // increment line counter
    ld a, (#LINECTR)
    inc a
    and a, #0x3F        ; mod 64
    ld (#LINECTR), a     ; store line counter value
    __endasm;
}

void delay(int count)
{
    int i;
    for (i = 0; i < count; i++)
    {
        __asm
        nop
        __endasm;
    }
}

void drawToScreen(unsigned char* scrbuf)
{
    //clear();
    print(scrbuf);
}

void enableReset()
{
    // clear the program loaded marker, enabling reloading on
    // warm boot
    unsigned char* marker = (unsigned char*)PROGRAM_LOADED_ADDR;
    *marker = 0;
}

/*
void memtest()
{
    unsigned char str[10];
    unsigned char* addr;
    unsigned char* top;
    unsigned char ch;
    unsigned char test;
    unsigned int result;
    print("Memory test..");
    newline();

    addr = (unsigned char*)2048;
    top = (unsigned char*)65535;

    while (addr < top)
    {
        ch = 170;
        test = 0;
        *addr = 0;
        *addr = ch;
        test = *addr;

        if (test != ch)
        {
            break;
        }

        addr++;
        print(".");
    }

    result = (int)addr;
    print("top was: ");
    _uitoa(result, str, 10);
    print(str);
    newline();
}
*/

void main()
{
    unsigned char* lctr;
    unsigned char* scrbuf;
    char i;
    char j;
    unsigned char ss[12];
    unsigned char *str;
    char x, y;
    unsigned char tt;
    //float radius;
    //float x, y;
    lctr = (unsigned char*)LINECTR;
    scrbuf = (unsigned char*)SCREENMEM;
    tt = 0;

    enableReset();
    clear();

    print("alloc test: ");
    str = malloc(12);
    _uitoa((unsigned int)str, ss, 10);
    print(ss);
    newline();
    readString(str);
    print("got: ");
    print(str);
    free(str);

    //memtest();

    /*
    while (1)
    {
        for (i = 0; i < 16; i++)
        {
            for (j = 0; j < 64; j++)
            {
                x = j - 32;
                y = i - 8;

                if (x < 0) x = -x;
                if (y < 0) y = -y;

                if (x < 7 && y < 7 && (y == tt || (x == 6 && y < tt) ))
                // && x == tt)
                {
                    *scrbuf = '*';
                }
                else
                {
                    *scrbuf = ' ';
                }

                scrbuf++;
            }
        }


        *scrbuf = 0;
        scrbuf = (unsigned char*)SCREENMEM;

        tt++;
        if (tt >= 7)
        {
            tt = 0;
        }

        drawToScreen(scrbuf);
    }
    */

    /*
    while(1)
    {
        print("Type a string:");
        newline();
        print(">");
        readString(str);
        print("you typed: ");
        print(str);
        newline();
    }
    */




    /*
    clear();
    newline();
    for (j = 0; j < 10; j++)
    {
        for (i = 0; i < 10; i++)
        {
            _uitoa(i, str, 10);
            print(str);
        }
        newline();
    }

    // read and echo a string
    print("Reading.");
    newline();
    while(1)
    {
        ch = 0;
        while(ch == 0)
        {
            ch = readchar();
        }

        if (ch == 13)
        {
            newline();
        }
        else
        {
            str[0] = ch;
            str[1] = 0;
            print(str);
        }
        while (ch != 0)
        {
            ch = readchar();
        }
    }
    */
}
