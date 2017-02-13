#include <stdlib.h>

//void printchar(unsigned char c);
#define LINECTR 0x0140
#define TEMP 0x0151

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

    return (*temp & 0x7F);
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

/*
void printchar(unsigned char c)
{
    c;
    __asm
    ld	iy,#2
    add	iy,sp
    ld	a,0 (iy)
    call #0x00FA
    __endasm;
}
*/
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

void main()
{
    unsigned char* lctr;
    unsigned int i;
    unsigned int j;
    unsigned char str[12];
    unsigned char num = 123;
    unsigned char ch;
    lctr = (unsigned char*)LINECTR;

    clear();

/*
    print("X");
    print("X");
    print("X");
    print("X");

    ch = *lctr;
    _uitoa(ch, str, 10);
    print(str);

    newline();

    print("Y");
    ch = *lctr;
    _uitoa(ch, str, 10);
    print(str);
*/

/*
    for (j = 0; j < 100; j++)
    {
        print("y");
    }

    ch = *lctr;
    _uitoa(ch, str, 10);
    print(str);
*/


    newline();
    for (j = 0; j < 10; j++)
    {
        for (i = 0; i < 10; i++)
        {
            _uitoa(i, str, 10);
            print(str);
        }
        newline();
        //clear();
    }

    /*
    while(1)
    {
        print("*** ");
        ch = readchar();
        _uitoa(ch, str, 10);
        print(str);
        delay(10000);
        newline();
    }
    */


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
            /*
            print(" ");
            _uitoa(ch, str, 10);
            print(str);
            newline();
            */
        }
        while (ch != 0)
        {
            ch = readchar();
        }
    }


/*
    while(1)
    {
        ch = readchar();
        _uitoa(ch, str, 10);
        print(str);
        newline();
        delay(1000);
    }
*/

    /*
    while(1)
    {
        print("Reading..");
        newline();
        ch = 0;
        while(ch == 0)
        {
            ch = readchar();
            delay(10000);
            print("***");
            newline();
        }

        newline();
        print("What's up, you typed ");
        str[0] = ch;
        str[1] = 0;
        print(str);
        print(" ");
        _uitoa(ch, str, 10);
        print(str);
        newline();
    }
    */

}
