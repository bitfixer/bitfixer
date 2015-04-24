
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <c64.h>

#define VICCTRL     53272
#define VICREG      53265
#define BGCOLOR     53281
#define SCREENMEM   1024
#define COLORMEM    55296

int main(void)
{
    //printf("hey there");
    
    // set to bitmap mode
    int base = 8192;
    unsigned char *vicCtrlReg;
    unsigned char *vicReg;
    unsigned char *memptr;
    unsigned char temp;
    int x;
    int i;
    
    unsigned char y;
    unsigned char ch;
    unsigned char ro;
    unsigned char ln;
    int by;
    unsigned char bi;
    
    vicCtrlReg = (unsigned char *)VICCTRL;
    vicReg = (unsigned char *)VICREG;
    
    // put bitmap at 8192
    temp = *vicCtrlReg;
    temp |= 8;
    *vicCtrlReg = temp;
    
    // set to bitmap mode
    temp = *vicReg;
    temp |= 32;
    *vicReg = temp;
    
    // set background color to black
    *(unsigned char *)BGCOLOR = 0;
    
    /*
    memptr = (unsigned char *)base;
    for (i = 0; i < 7999; i++)
    {
        *memptr = 0;
        memptr++;
    }
    */
    
    memptr = (unsigned char *)base;
    memset(memptr, 0, 8000);

    
    memptr = (unsigned char *)SCREENMEM;
    memset(memptr, 247, 1000);
    /*
    for (i = 0; i < 1000; i++)
    {
        *memptr = 247;
        memptr++;
    }
    */
    memptr = (unsigned char *)COLORMEM;
    memset(memptr, 10, 1000);
    /*
    for (i = 0; i < 1000; i++)
    {
        *memptr = 10;
        memptr++;
    }
    */
    
    memptr = (unsigned char *)base;
    temp = 0;
    
    
    
    
    /*
    for (i = 0; i < 240; i++)
    {
        memset(memptr, temp, 8000);
        temp++;
    }
    */
    
    
    
    /*
    for (x = 0; x < 320; x += 1)
    {
        y = (unsigned char)(90.0 + 80.0*sin(x/10.0));
        ch = (unsigned char)(x/8);
        ro = (unsigned char)(y/8);
        ln = y & 7;
        by = base+ro*320+8*ch+ln;
        bi = 7 - ((int)x & 7);
        memptr = (unsigned char *)by;
        temp = *memptr;
        temp |= 1<<bi;
        *memptr = temp;
    }
    */
    
    return 0;
}