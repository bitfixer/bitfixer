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

int softSpi::send(unsigned char* buffer, int size)
{
    unsigned char c = 1;
    unsigned char clk;
    unsigned char cs;
    unsigned char bits = 0;
    unsigned char byte = 0;
    unsigned char sendbyte = 0xAA;
    int bytesReceived = 0;
    
    if (_driveChipSelect)
    {
        // lower chip select line
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
    
    for (int s = 0; s < size && c == 0; s++)
    {
        byte = 0;
        for (unsigned char i = 0; i < 8; i++)
        {
            // present bit
            setMiso(sendbyte & 0x80);
            sendbyte <<= 1;
            
            while (clk == 0x00 && c == 0)
            {
                clk = PINB & SCKMASK;
                if (!_driveChipSelect)
                {
                    c = PINB & CSMASK;
                }
            }
            
            if (c != 0)
            {
                break;
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
        
        if (c == 0)
        {
            buffer[s] = byte;
            bytesReceived++;
        }
    }
    
    if (_driveChipSelect)
    {
        PORTB = CSMASK;
    }
    
    return bytesReceived;
}
