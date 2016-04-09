#include "serial.h"
#include <c64.h>

unsigned char test()
{
    return 234;
}

void serial_init()
{
    unsigned char temp;
    // set portb (userport data) to input
    CIA2.ddrb = 0x00;
    
    // set handshake line PA2 to output
    CIA2.ddra = PA2;
    
    // set PA2 high
    CIA2.pra = PA2;
    
    // read ICR to clear flag
    temp = CIA2.icr;
}

unsigned char serial_byte_ready()
{
    unsigned char data = CIA2.icr & FLAG;
    if (data == 0)
        return 0;
    
    return 1;
}

unsigned char serial_read_byte()
{
    unsigned char data = CIA2.prb;
    CIA2.pra = 0x00;
    return data;
}

void serial_done_reading()
{
    CIA2.pra = PA2;
}