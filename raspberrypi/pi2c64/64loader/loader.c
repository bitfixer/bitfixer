#include <stdio.h>
#include <c64.h>
#include <peekpoke.h>

#define PA2 0x04;
#define NPA2 0xFB;

#define FLAG 0x10;

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

int main(void)
{
    /*
    unsigned char byte;
    int i = 0;
    // TEST
    init();
    set_userport_input();
    for (i = 0; i < 255; i++)
    {
        byte = receive_byte();
        printf("received %d\n", byte);
        
    }
    return 1;
    */
    
    unsigned i = 0;
    unsigned char c;
    init();
    
    // test
    
    printf("start\n");
    for (i = 0; i < 255; i++)
    {
        asm("nop");
    }
    
    for (i = 0; i < 1; i++)
    {
        for (c = 0; c < 255; c++)
        {
            send_byte_with_handshake(c);
        }
    }
    
    send_byte_with_handshake(0xff);
    
    // now receive some bytes
    set_userport_input();
    for (i = 0; i < 255; i++)
    {
        c = receive_byte_with_handshake();
        //printf("received %d\n", c);
    }
    
    signal_byte_not_ready();
    set_userport_output();
    printf("done.\n");
    return 1;
}
