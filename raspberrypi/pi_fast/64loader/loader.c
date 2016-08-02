#include <stdio.h>
#include <c64.h>
#include <peekpoke.h>
#include <string.h>

#define PA2 0x04
#define NPA2 0xFB

#define FLAG 0x10
#define SCREENRAM 0x0400
#define COLORRAM 0xD800
#define BITMAP_LOCATION_REG 0xD018
#define VICII 0xD011
#define BORDER 0xD020
#define KEYCHECK 0xDC01
#define KEYPRESS 0x00C5

typedef enum
{
    false = 0,
    true = 1
} bool;

void set_userport_output()
{
    CIA2.ddrb = 0xff;
}

void set_userport_input()
{
    CIA2.ddrb = 0x00;
}

void signal_byte_ready()
{
    // lower PA2 bit
    unsigned char val = CIA2.pra;
    val = val & NPA2;
    CIA2.pra = val;
}

void signal_byte_not_ready()
{
    unsigned char val = CIA2.pra;
    // raise PA2
    val = val | PA2;
    CIA2.pra = val;
}

void transmit_byte(unsigned char byte)
{
    CIA2.prb = byte;
}

unsigned char receive_byte()
{
    return CIA2.prb;
}

void set_border_color(unsigned char color_index)
{
    *((unsigned char *)BORDER) = color_index;
}

void init()
{
    
    // set port register a to output for bit 2
    unsigned char reg = CIA2.ddra;
    // raise bit 2
    reg = reg | PA2;
    CIA2.ddra = reg;
    
    // lower bit 2
    // set PA2 line to input
    //reg = reg & NPA2;
    //CIA2.ddra = reg;
    
    //set_userport_input(); // get ready to read bytes
    set_userport_output();
}

void send_command(unsigned char cmd)
{
    // place byte on userport
    transmit_byte(cmd);
    
    // signal ready
    signal_byte_ready();
    
    // wait for flag
    // wait for FLAG
wait:
    asm("lda %w", 0xDD0D);
    asm("and #$10");
    asm("beq %g", wait);
    
    signal_byte_not_ready();
}

void load_mem(unsigned addr, unsigned char pages)
{
    unsigned char lo;
    unsigned char hi;
    
    lo = addr & 0x00FF;
    hi = (addr >> 8) & 0x00FF;
    
    // store destination address
    *((unsigned char *)0x00FB) = lo;
    *((unsigned char *)0x00FC) = hi;
    
    // store number of pages
    *((unsigned char *)0x00FD) = pages;
    
    // clear FLAG
    asm("lda %w", 0xDD0D);
    
    asm("ldx $FD");
    asm("ldy #$00");
    
loadpage:
    // lower PA2
    asm("lda %w", 0xDD00);
    asm("and #$FB");
    asm("sta %w", 0xDD00);
    
    // wait for FLAG
wait:
    asm("lda %w", 0xDD0D);
    asm("and #$10");
    asm("beq %g", wait);
    
loop:
    // load byte
    asm("lda %w", 0xDD01);
    asm("sta ($FB), y");
    asm("iny");
    asm("lda %w", 0xDD01);
    asm("sta ($FB), y");
    asm("iny");
    asm("lda %w", 0xDD01);
    asm("sta ($FB), y");
    asm("iny");
    asm("lda %w", 0xDD01);
    asm("sta ($FB), y");
    asm("iny");
    asm("lda %w", 0xDD01);
    asm("sta ($FB), y");
    asm("iny");
    asm("lda %w", 0xDD01);
    asm("sta ($FB), y");
    asm("iny");
    asm("lda %w", 0xDD01);
    asm("sta ($FB), y");
    asm("iny");
    asm("lda %w", 0xDD01);
    asm("sta ($FB), y");
    asm("iny");
    asm("lda %w", 0xDD01);
    asm("sta ($FB), y");
    asm("iny");
    asm("lda %w", 0xDD01);
    asm("sta ($FB), y");
    asm("iny");
    asm("lda %w", 0xDD01);
    asm("sta ($FB), y");
    asm("iny");
    asm("lda %w", 0xDD01);
    asm("sta ($FB), y");
    asm("iny");
    asm("lda %w", 0xDD01);
    asm("sta ($FB), y");
    asm("iny");
    asm("lda %w", 0xDD01);
    asm("sta ($FB), y");
    asm("iny");
    asm("lda %w", 0xDD01);
    asm("sta ($FB), y");
    asm("iny");
    asm("lda %w", 0xDD01);
    asm("sta ($FB), y");
    asm("iny");
    asm("bne %g", loop);
    
    // decrement page count
    asm("inc $FC");
    asm("dex");
    asm("bne %g", loop);
    
    // raise PA2
    asm("lda %w", 0xDD00);
    asm("ora #$04");
    asm("sta %w", 0xDD00);
}

int main(void)
{
    unsigned char val;
    int i;
    bool done = false;
    
    init();
    
    //
    memset((unsigned char *)COLORRAM, 0, 1000);
    
    /*
    // read 2 bytes: C
    val = CIA2.pra & PA2;
    // wait for PA2 to go low
    while (val != 0)
    {
        val = CIA2.pra & PA2;
    }
    
    // read a value
    *((unsigned char *)SCREENRAM+80) = CIA2.prb;
    
    val = CIA2.pra & PA2;
    while (val == 0)
    {
        val = CIA2.pra & PA2;
    }
    
    *((unsigned char *)SCREENRAM+81) = CIA2.prb;
    */
    
    /*
    // read 2 bytes: asm
    asm("ldx #$00");
wait1:
    asm("lda %w", 0xDD00);
    asm("and #$04");
    asm("bne %g", wait1);       // loop if PA2 is not low
    
    // read one byte
    asm("lda %w", 0xDD01);
    asm("sta %w,x", SCREENRAM+80);
    
    asm("inx");
    asm("beq %g", done);
    
wait2:
    asm("lda %w", 0xDD00);
    asm("and #$04");
    asm("beq %g", wait2);
    
    asm("lda %w", 0xDd01);
    asm("sta %w,x", SCREENRAM+80);
    
    asm("inx");
    asm("bne %g", wait1);

done:
    asm("nop");
    */
    
    /*
    // read 256 bytes - speed test
    asm("ldy #$00");
    
    // store destination address
    asm("lda #$00");
    asm("sta %w", 0x00FB);
    asm("lda #$04");
    asm("sta %w", 0x00FC);
    
    // lower PA2
    asm("lda %w", 0xDD00);
    asm("and #$FB");
    asm("sta %w", 0xDD00);
loop:
    
    // load byte
    asm("lda %w", 0xDD01);
    //asm("sta %w,x", SCREENRAM);
    asm("sta ($FB), y");
    asm("iny");
    asm("bne %g", loop);
    
    asm("lda %w", 0xDD00);
    asm("ora #$04");
    asm("sta %w", 0xDD00);
    */
    
    // send command
    
    for (i = 0; i < 26; i++)
    {
        send_command(i);
        load_mem(SCREENRAM, 4);
    }
    
    /*
    *((unsigned char *)COLORRAM) = 0;
    while (!done)
    {
        // read PA2
        val = CIA2.pra;
        val = val & PA2;
        if (val == 0)
        {
            *((unsigned char *)SCREENRAM) = '0';
        }
        else
        {
            *((unsigned char *)SCREENRAM) = '1';
        }
        
        // check for keypress
        val = *(unsigned char *)KEYPRESS;
        if (val == 62)
        {
            // quit
            done = true;
        }
    }
    */
     
    return 1;
}
