#include "serial.h"
#include <peekpoke.h>

unsigned char test()
{
    return 234;
}

void serial_init()
{
    unsigned char temp;
    // set portb (userport data) to input
    POKE(DDRB, 0x00);
    
    // set handshake line PA2 to output
    POKE(DDRA, PA2);
    
    // set PA2 high
    POKE(PORTA, PA2);
    
    // read ICR to clear flag
    temp = PEEK(ICR);
}

unsigned char serial_byte_ready()
{
    unsigned char data = PEEK(ICR) & FLAG;
    if (data == 0)
        return 0;
    
    return 1;
}

unsigned char serial_read_byte()
{
    unsigned char data = PEEK(PORTB);
    POKE(PORTA, 0x00);
    return data;
}

void serial_done_reading()
{
    POKE(PORTA, PA2);
}