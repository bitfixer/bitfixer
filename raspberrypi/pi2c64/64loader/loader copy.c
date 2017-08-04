#include <stdio.h>
#include <c64.h>
#include <peekpoke.h>

int main(void)
{
    /*
    unsigned i = 0;
    unsigned char *sid;
    unsigned addr;
    printf("Hello!");
    printf("you are cool.");
    
    for (i = 0; i < 10; i++)
    {
        printf("number %d\n", i);
    }
    
    // play sid sound
    sid = (unsigned char *)SID;
    
    for (i = 0; i < 28; i++)
    {
        POKE((unsigned)SID+i, 0);
    }
    
    POKE((unsigned)SID+24, 15);
    POKE((unsigned)SID+1, 20);
    POKE((unsigned)SID+5, 0*16+0);
    POKE((unsigned)SID+6, 15*16+9);
    POKE((unsigned)SID+4, 1+16);
    POKE((unsigned)SID+4, 16);
    */
    
    /*
    unsigned char *tmp;
    unsigned char *sid = (unsigned char *)SID;
    tmp = sid + 24;
    
    *tmp = 15;
    
    POKE(0xD418, 15);
    */
    //struct __sid *theSID = &SID;
    //theSID->amp = 15;
    
    unsigned char byte;
    
    // set userport to input
    CIA2.ddrb = 0;
    
    SID.amp = 15;
    SID.v1.freq = 5120;
    SID.v1.ad = 0;
    SID.v1.sr = 15*16+9;
    
    SID.v2.freq = 9000;
    SID.v2.ad = 0;
    SID.v2.sr = 15*16+9;
    
    SID.v3.freq = 2000;
    SID.v3.ad = 0;
    SID.v3.sr = 15*16+9;
    
    SID.v1.ctrl = 1+16;
    
    while (1)
    {
        byte = CIA2.prb;
        byte -= 49;
        SID.v1.freq = byte*256;
    }
    
    /*
    while (1)
    {
        byte = CIA1.prb;
        if (byte == 0xbf)
        {
            SID.v1.ctrl = 1+16;
            SID.v1.ctrl = 16;
            
            SID.v2.ctrl = 1+16;
            SID.v2.ctrl = 16;
            
            SID.v3.ctrl = 1+16;
            SID.v3.ctrl = 16;
        }
        else if (byte == )
    }
    */
    
    
    
    
    return 1;
}
