
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <c64.h>

#define CIA2PORTA   56576
#define CIA2PORTB   56578
#define VICCTRL     53272
#define RASTERREG   53266
#define VICREG      53265
#define BGCOLOR     53281
#define SCREENMEM   1024
//#define SCREENMEM   7168
#define COLORMEM    55296

#define VRAMBANK    32768
#define BACKPAGE    16384

int main(void)
{
    //printf("hey there");
    
    // set to bitmap mode
    int base = 8192+VRAMBANK;
    unsigned char *vicCtrlReg;
    unsigned char *cia2porta;
    unsigned char *cia2portb;
    unsigned char *vicReg;
    unsigned char *memptr;
    unsigned char *rasterReg;
    unsigned char *backpage;
    unsigned char temp;
    unsigned char temp2;
    unsigned char diff;
    int x;
    int i;
    int line;
    
    /*
    unsigned char y;
    unsigned char ch;
    unsigned char ro;
    unsigned char ln;
    int by;
    unsigned char bi;
    */
     
    cia2porta = (unsigned char *)CIA2PORTA;
    cia2portb = (unsigned char *)CIA2PORTB;
    vicCtrlReg = (unsigned char *)VICCTRL;
    vicReg = (unsigned char *)VICREG;
    rasterReg = (unsigned char *)RASTERREG;
    
    // set bits 0 and 1 to outputs port a cia2
    temp = *cia2portb;
    temp |= 3;
    *cia2portb = temp;
    
    // select video bank
    temp = *cia2porta;
    temp &= 252;
    temp |= 1;
    *cia2porta = temp;
    
    /*
    // move screen memory
    temp = *vicCtrlReg;
    temp &= 15;
    temp |= 112;
    *vicCtrlReg = temp;
    */
     
    // put bitmap at 32768
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

    
    memptr = (unsigned char *)(SCREENMEM+VRAMBANK);
    memset(memptr, 247, 1000);
    
    memptr = (unsigned char *)(SCREENMEM+BACKPAGE);
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
    /*
    for (i = 0; i < 10; i++)
    {
        for (x = 0; x <= 312; x+=8)
        {
            memptr[x + i] = 0xff;
        }
    }
    */
    
    /*
    for (i = 0; i < 8000; i++)
    {
        memptr[i] = 0xff;
    }
    */
    
    backpage = (unsigned char *)(BACKPAGE+8192);
    temp = 0;
    for (i = 0; i < 15; i++)
    {
        // write to back page
        memset(backpage, temp, 8000);
        
        // swap page
        temp = *cia2porta;
        temp &= 252;
        temp |= 2;
        
        // wait for blank
        loop:
            asm("lda $d012");
            asm("cmp #$ff");
            asm("bne %g", loop);
        
        *cia2porta = temp;
        
        // write to front page
        temp = 0xff;
        memset(memptr, temp, 8000);
        
        // swap page
        temp = *cia2porta;
        temp &= 252;
        temp |= 1;
        
        // wait for blank
        loop2:
            asm("lda $d012");
            asm("cmp #$ff");
            asm("bne %g", loop2);
        
        *cia2porta = temp;
        temp = 0;
    }
    
    
    
    /*
    temp = 0;
    for (i = 0; i < 100; i++)
    {
        
        loop:
            asm("lda $d012");
            asm("cmp #$ff");
            asm("bne %g", loop);
        
        if (temp == 0)
        {
            asm("ldx #$00");
            asm("lda #$00");
            
            update:
                asm("sta %w,x", 0xA000);
                asm("inx");
                asm("bne %g", update);
            updateb:
                asm("sta %w,x", 0xA100);
                asm("inx");
                asm("bne %g", updateb);
            updatec:
                asm("sta %w,x", 0xA200);
                asm("inx");
                asm("bne %g", updatec);
            updated:
                asm("sta %w,x", 0xA300);
                asm("inx");
                asm("bne %g", updated);
            updatee:
                asm("sta %w,x", 0xA400);
                asm("inx");
                asm("bne %g", updatee);
            
        }
        else
        {
            asm("ldx #$00");
            asm("lda #$FF");
            
            update2:
                asm("sta %w,x", 0xA000);
                asm("inx");
                asm("bne %g", update2);
            updateb2:
                asm("sta %w,x", 0xA100);
                asm("inx");
                asm("bne %g", updateb2);
            updatec2:
                asm("sta %w,x", 0xA200);
                asm("inx");
                asm("bne %g", updatec2);
            updated2:
                asm("sta %w,x", 0xA300);
                asm("inx");
                asm("bne %g", updated2);
            updatee2:
                asm("sta %w,x", 0xA400);
                asm("inx");
                asm("bne %g", updatee2);
            
        }
        
        //memset(memptr, temp, 640);
        temp = ~temp;
    }
    */
     
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