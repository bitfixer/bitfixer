#include <stdio.h>
#include <c64.h>
#include <peekpoke.h>
#include "../commands.h"

#define PA2 0x04
#define NPA2 0xFB

#define FLAG 0x10
#define SCREENRAM 0x0400
#define BITMAP_LOCATION_REG 0xD018
#define VICII 0xD011

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
    unsigned char val = CIA2.icr & FLAG;
    while (val == 0x00)
    {
        val = CIA2.icr & FLAG;
        //asm("nop");
    }
}

void wait_for_remote_notready()
{
    unsigned char val = CIA2.icr & FLAG;
    while (val != 0x00)
    {
        val = CIA2.icr & FLAG;
        //asm("nop");
    }
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
 
int main(void)
{
    // TEST: bitmap mode
    unsigned base = 8192;
    unsigned char val;
    unsigned char *addr;
    unsigned i;
    
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
    
    for (i = base; i < base+8000; i++)
    {
        *((unsigned char *)i) = 0;
    }
    
    for (i = 1024; i < 2023; i++)
    {
        *((unsigned char *)i) = 0x25;
    }
    
    send_command(COMMAND_GET_FRAME);
    set_userport_input();
    for (i = base; i < base+8000; i++)
    {
        val = receive_byte_with_handshake();
        *((unsigned char *)i) = val;
    }
    set_userport_output();
    
    
    signal_byte_not_ready();
    set_userport_output();
   
    for (i = 0; i < 1000; i++)
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
    
    /*
    unsigned i = 0;
    unsigned j = 0;
    unsigned char c;
    unsigned char *screen = (unsigned char *)SCREENRAM;
    for (j = 0; j < 1000; j++)
    {
        COLOR_RAM[j] = 0;
    }
    
    for (j = 0; j < 1000; j++)
    {
        asm("nop");
    }
    
    init();
    for (i = 0; i < 10; i++)
    {
        send_command(COMMAND_GET_FRAME);
    
        set_userport_input();
        // get response
        for (j = 0; j < 1000; j++)
        {
            //c = receive_byte_with_handshake();
            c = receive_byte_with_handshake();
            //printf("received %d\n", c);
            screen[j] = c;
        }
        set_userport_output();
    }
    
    signal_byte_not_ready();
    set_userport_output();
    //printf("done.\n");
    return 1;
    */
}
