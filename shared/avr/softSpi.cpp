//
//  softSpi.cpp
//  
//
//  Created by Michael Hill on 4/8/17.
//
//

extern "C"
{
#include <avr/io.h>
#include "UART_routines.h"
}

#include "softSpi.h"
#include <util/delay.h>

#define CSMASK  0x04
#define SCKMASK 0x20
#define MOSIMASK 0x10
#define MISOMASK 0x08

void softSpi::init()
{
    if (_driveChipSelect)
    {
        DDRB = CSMASK | MISOMASK;
        PORTB = CSMASK;
    }
    else
    {
        DDRB = MISOMASK;
        PORTB = 0x00;
    }
}

void softSpi::setMiso(unsigned char bit)
{
    unsigned char c;
    c = PORTB;
    if (bit == 0)
    {
        c = c & ~MISOMASK;
    }
    else
    {
        c = c | MISOMASK;
    }
    PORTB = c;
}

void softSpi::send(unsigned char sendbyte)
{
    unsigned char c = 1;
    unsigned char clk;
    unsigned char bits = 0;
    unsigned char byte = 0;
    
    if (_driveChipSelect)
    {
        // lower chip select line
        //c = PORTB;
        //c = c & ~CSMASK;
        //PORTB = c;
        PORTB = 0x00;
    }
    else // read chip select from master
    {
        while (c != 0)
        {
            c = PINB & CSMASK;
        }
    }
    
    clk = PINB & SCKMASK;
    
    for (unsigned char i = 0; i < 8; i++)
    {
        // present bit
        setMiso(sendbyte & 0x80);
        sendbyte <<= 1;
        
        while (clk == 0x00)
        {
            clk = PINB & SCKMASK;
        }
        
        byte <<= 1;
        if ((PINB & MOSIMASK) != 0x00)
        {
            byte = byte | 0x01;
        }
        
        while (clk != 0x00)
        {
            clk = PINB & SCKMASK;
        }
    }
    
    if (_driveChipSelect)
    {
        //c = PORTB;
        //c = c | CSMASK;
        //PORTB = c;
        PORTB = CSMASK;
    }
    
    transmitString((unsigned char *)"read cs data: ");
    transmitHex(CHAR, byte);
    transmitString((unsigned char*)"\r\n");
}
