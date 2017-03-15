#include <stdio.h>
#include <c64.h>
#include <peekpoke.h>
#include <string.h>
//#include "../commands.h"

#define PA2 0x04
#define NPA2 0xFB

#define FLAG 0x10
#define SCREENRAM 0x0400
#define COLORRAM 0xD800
#define BITMAP_LOCATION_REG 0xD018
#define VICII 0xD011
#define VICII_REG2 0xD016
#define BORDER 0xD020
#define BGCOLOR 0xD021
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
    *((unsigned char*)BORDER) = color_index;
}

void set_background_color(unsigned char color_index)
{
    *((unsigned char*)BGCOLOR) = color_index;
}

void init()
{

    // set port register a to output for bit 2
    unsigned char reg = CIA2.ddra;
    // raise bit 2
    reg = reg | PA2;
    CIA2.ddra = reg;

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
    asm("lda %w", KEYPRESS);
    asm("cmp #$3E"); // Q
    asm("beq %g", quit);

    asm("lda %w", 0xDD0D);
    asm("and #$10");
    asm("beq %g", wait);
quit:

    signal_byte_not_ready();
}

void set_vic_bank(unsigned char bank)
{
    unsigned char val;
    unsigned char mask = 3-bank;

    val = CIA2.pra;
    val = (val & 0xFC) | mask;
    CIA2.pra = val;
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
    asm("lda %w", KEYPRESS);
    asm("cmp #$3E"); // Q
    asm("beq %g", quit);

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
quit:
    return;
}

void standardBitmapMode()
{
    unsigned char* addr;
    unsigned char val;
    // enter bit map mode
    addr = (unsigned char *)VICII;
    val = *addr;
    val |= 0x20; // raise bit 5
    *addr = val;

    addr = (unsigned char*)VICII_REG2;
    val = *addr;
    val &= 0xDF; // lower bit 5
    *addr = val;
}

void multicolorBitmapMode()
{
    unsigned char* addr;
    unsigned char val;
    // enter bit map mode
    addr = (unsigned char *)VICII;
    val = *addr;
    val |= 0x20; // raise bit 5
    *addr = val;

    addr = (unsigned char*)VICII_REG2;
    val = *addr;
    val |= 0x10; // raise bit 4
    *addr = val;
}

int main(void)
{
    unsigned base = 8192;
    unsigned page = 16384;
    unsigned char val;
    unsigned char *addr;
    unsigned char currpage;
    int i;
    int j;
    bool done = false;

    init();

    // set bitmap at 8192
    addr = (unsigned char *)BITMAP_LOCATION_REG;
    val = *addr;
    val |= 0x08;
    *addr = val;

    standardBitmapMode();
    //multicolorBitmapMode();

    memset((unsigned char *)base, 0, 8000);
    memset((unsigned char *)SCREENRAM, 0, 1000);
    memset((unsigned char *)base+page, 0, 8000);
    memset((unsigned char *)SCREENRAM+page, 0, 1000);

    currpage = 0;
    set_border_color(0);
    //set_background_color(2);
    set_vic_bank(1);
    //set_vic_bank(0);

    /*
    memset((unsigned char*)SCREENRAM, 0x0F, 1000);

    for (i = 0; i < 128; i++)
    {
        addr = (unsigned char *)(base + i);
        *addr = 0x06;
    }
    */

    // send command
    //for (i = 0; i < 60; i++)
    while (1)
    {
        // check for keypress
        val = *(unsigned char *)KEYPRESS;
        if (val == 62)
        {
            // quit
            break;
        }

        send_command(0);
        set_userport_input();
        load_mem(SCREENRAM+(currpage*page), 4);
        set_userport_output();

        for (j = 1; j < 9; j++)
        {
            send_command(j);
            set_userport_input();
            load_mem(base+(currpage*page)+((j-1)*1024), 4);
            set_userport_output();
        }

        // flip the page
        set_vic_bank(currpage);
        currpage++;
        if (currpage > 1)
            currpage = 0;
    }

    set_vic_bank(0);
    return 1;
}
