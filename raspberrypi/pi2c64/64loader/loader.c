#include <stdio.h>
#include <c64.h>
#include <peekpoke.h>
#include <string.h>
#include "../commands.h"
#include "asmloaders.h"

#define PA2 0x04
#define NPA2 0xFB

#define FLAG 0x10
#define SCREENRAM 0x0400
#define BITMAP_LOCATION_REG 0xD018
#define VICII 0xD011
#define BORDER 0xD020
#define KEYCHECK 0xDC01

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

void wait_for_remote_ready()
{
    /*
    unsigned char count;
    unsigned char val;
    
    count = 0;
    while (count < 10)
    {
        //printf("c %d\n", count);
        val = CIA2.icr & FLAG;
        if (val == 0x00)
        {
            count = 0;
        }
        else
        {
            count++;
        }
    }
    */
    
    //dd0d
    /*
    unsigned char val = CIA2.icr & FLAG;
    while (val == 0x00)
    {
        val = CIA2.icr & FLAG;
        //asm("nop");
    }
    */
    
loopstart:
    asm("lda %w", 0xDD0D);
    asm("and #$10");
    asm("beq %g", loopstart);
loop2:
    asm("lda %w", 0xDD0D);
    asm("and #$10");
    asm("bne %g", loop2);
}

void wait_for_remote_notready()
{
    /*
    unsigned char count;
    unsigned char val;
    count = 0;
    while (count < 10)
    {
        val = CIA2.icr & FLAG;
        if (val == 0x00)
        {
            count++;
        }
        else
        {
            count = 0;
        }
    }
    */
    
    /*
    unsigned char val = CIA2.icr & FLAG;
    while (val != 0x00)
    {
        val = CIA2.icr & FLAG;
        //asm("nop");
    }
    */
    
    /*
    unsigned char val = CIA2.icr & FLAG;
    while (val == 0x00)
    {
        val = CIA2.icr & FLAG;
        //asm("nop");
    }
    */
    
    /*
    int i;
    for (i = 0; i < 10; i++)
    {
        asm("nop");
    }
    */
    
loopstart:
    asm("lda %w", 0xDD0D);
    asm("and #$10");
    asm("beq %g", loopstart);
loop2:
    asm("lda %w", 0xDD0D);
    asm("and #$10");
    asm("bne %g", loop2);
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
    
    signal_byte_not_ready();
    set_userport_output();
    transmit_byte(0x00);
    
    set_border_color(0);
}

void send_byte_with_handshake(unsigned char byte)
{
    transmit_byte(byte);
    signal_byte_ready();
    wait_for_remote_ready();
    signal_byte_not_ready();
    wait_for_remote_notready();
}

unsigned char receive_byte_with_handshake()
{
    unsigned char byte;
    signal_byte_ready();
    wait_for_remote_ready();
    byte = receive_byte();
    signal_byte_not_ready();
    wait_for_remote_notready();
    return byte;
}

void send_command(unsigned char command_id)
{
    send_byte_with_handshake(command_id);
}

void load_color_mem()
{
    // counter
    asm("ldx #$00");
    
    // trigger
    asm("lda %w", 0xDD00);
    asm("tay");
loop:
    // signal byte ready
    asm("tya");
    asm("and #$FB");
    asm("sta %w", 0xDD00);
    
    // wait for remote
wait1:
    asm("lda %w", 0xDD0D);
    asm("and #$10");
    asm("beq %g", wait1);
    
    // load byte
    asm("lda %w", 0xDD01);
    asm("sta %w,x", SCREENRAM);
    
    // signal not ready
    asm("tya");
    asm("ora #$04");
    asm("sta %w", 0xDD00);
    
    // wait for remote2
wait11:
    asm("lda %w", 0xDD0D);
    asm("and #$10");
    asm("beq %g", wait11);
    
    // inc pointer
    asm("inx");
    asm("bne %g", loop);
    
loop2:
    // signal byte ready
    asm("tya");
    asm("and #$FB");
    asm("sta %w", 0xDD00);
    
    // wait for remote
wait2:
    asm("lda %w", 0xDD0D);
    asm("and #$10");
    asm("beq %g", wait2);
    
    // load byte
    asm("lda %w", 0xDD01);
    asm("sta %w,x", 0x0500);
    
    // signal not ready
    asm("tya");
    asm("ora #$04");
    asm("sta %w", 0xDD00);
    
    // wait for remote2
wait22:
    asm("lda %w", 0xDD0D);
    asm("and #$10");
    asm("beq %g", wait22);
    
    // inc pointer
    asm("inx");
    asm("bne %g", loop2);
    
loop3:
    // signal byte ready
    asm("tya");
    asm("and #$FB");
    asm("sta %w", 0xDD00);
    
    // wait for remote
wait3:
    asm("lda %w", 0xDD0D);
    asm("and #$10");
    asm("beq %g", wait3);
    
    // load byte
    asm("lda %w", 0xDD01);
    asm("sta %w,x", 0x0600);
    
    // signal not ready
    asm("tya");
    asm("ora #$04");
    asm("sta %w", 0xDD00);
    
    // wait for remote2
wait33:
    asm("lda %w", 0xDD0D);
    asm("and #$10");
    asm("beq %g", wait33);
    
    // inc pointer
    asm("inx");
    asm("bne %g", loop3);
    
    // offset for last section
    asm("ldx #$18");
loop4:
    // signal byte ready
    asm("tya");
    asm("and #$FB");
    asm("sta %w", 0xDD00);
    
    // wait for remote
wait4:
    asm("lda %w", 0xDD0D);
    asm("and #$10");
    asm("beq %g", wait4);
    
    // load byte
    asm("lda %w", 0xDD01);
    asm("sta %w,x", 0x06E8);
    
    // signal not ready
    asm("tya");
    asm("ora #$04");
    asm("sta %w", 0xDD00);
    
    // wait for remote2
wait44:
    asm("lda %w", 0xDD0D);
    asm("and #$10");
    asm("beq %g", wait44);
    
    // inc pointer
    asm("inx");
    asm("bne %g", loop4);
}
 
int main(void)
{
    // TEST: bitmap mode
    unsigned base = 8192;
    unsigned char val;
    unsigned char *addr;
    unsigned i;
    unsigned frame;
    
    init();
    
    // set bitmap at 8192
    addr = (unsigned char *)BITMAP_LOCATION_REG;
    val = *addr;
    val |= 0x08;
    *addr = val;
    
    // enter bit map mode
    addr = (unsigned char *)VICII;
    val = *addr;
    val |= 0x20;
    *addr = val;
    
    /*
    for (i = base; i < base+8000; i++)
    {
        *((unsigned char *)i) = 0;
    }
    */
    memset((unsigned char *)base, 0, 8000);
    memset((unsigned char *)1024, 0, 1000);
    
    /*
    for (i = base; i < base+320; i++)
    {
        *((unsigned char *)i) = 255;
    }
    
    return 1;
    */
    
    for (frame = 0; frame < 10; frame++)
    {
        send_command(COMMAND_GET_FRAME);
        set_userport_input();
        
        load_color_mem();
        load_image();
        /*
        for (i = 1024; i < 2024; i++)
        {
            val = receive_byte_with_handshake();
            *((unsigned char *)i) = val;
        }
        */
        
        /*
        for (i = base; i < base+8000; i++)
        {
            val = receive_byte_with_handshake();
            *((unsigned char *)i) = val;
        }
        */
        set_userport_output();
    }
   
    signal_byte_not_ready();
    set_userport_output();
    
    for (i = 0; i < 10000; i++)
    {
        asm("nop");
    }
    
    /*
    // exit bit map mode
    addr = (unsigned char *)VICII;
    val = *addr;
    val &= 0xDF;
    *addr = val;
    */
     
    return 1;
}
