#include <stdlib.h>

unsigned char global;

void printchar(unsigned char c);

void print(unsigned char* str)
{
    str;
    __asm
    loadstack:
        pop	bc
        pop	hl
        push	hl
        push	bc
    printchar:
        ld	a,(hl)
        cp a, #0x00
        jp z,done
        set	7, a
        call #0x00FA
        inc hl
        jp printchar
    done:
        nop
    __endasm;
}

/*
void char2str(unsigned char c, unsigned char* str)
{
    unsigned char digit;
    unsigned char* ch = str;
    unsigned char buf[3];
    unsigned char len = 0;
    while (c != 0)
    {
        digit = c % 10;
        //*ch = '0' + digit;
        //ch++;
        buf[len] = '0' + digit;

        c = (c - digit) / 10;
    }
    *ch = 0;
}
*/

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

void bbcc()
{
    /*
    unsigned char* c = (unsigned char*)(0x0400);
    unsigned char _x = 0xC5;
    //TEMP1 = _x;
    *c = _x;
    __asm
    ld a, (0x0400)
    call 0x00FA
    __endasm;
    return;
    */

/*
    int i;
    unsigned char c[12] = "HELLO WORLD";
    c[11] = 0;

    for (i = 0; i < 5; i++)
    {
        print(c);
    }
    return;
*/

    //unsigned char c[12] = "HELLO WORLD";
    //c[11] = 0;
    unsigned int i;
    unsigned char str[12];
    unsigned char num = 123;
    //char2str(num, str);
    //_uitoa(num, str, 10);
    for (i = 0; i < 100; i++)
    {
        _uitoa(i, str, 10);
        print(str);
        print(" ");
    }

}
