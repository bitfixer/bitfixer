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

void softSpi::init()
{
    //DDRB = 0xFF;
    DDRB = 0x00;
}

void softSpi::send()
{
    /*
    while (1)
    {
        _delay_loop_2(65535);
        _delay_loop_2(65535);
        PORTB = 0xFF;
        _delay_loop_2(65535);
        _delay_loop_2(65535);
        PORTB = 0x00;
    }
    */
    
    /*
    unsigned char c;
    while (1)
    {
        c = PINB;
        transmitHex(CHAR, c);
        transmitString((unsigned char*)"\r\n");
        _delay_loop_2(65535);
        _delay_loop_2(65535);
        _delay_loop_2(65535);
        _delay_loop_2(65535);
    }
    */
    unsigned char c = 1;
    unsigned char clk;
    unsigned char bits = 0;
    unsigned char byte = 0;
    while (c != 0)
    {
        c = PINB & CSMASK;
    }
    
    clk = PINB & SCKMASK;
    
    for (unsigned char i = 0; i < 8; i++)
    {
        while (clk == 0x00)
        {
            clk = PINB & SCKMASK;
        }
        
        byte <<= 1;
        if ((PINB & MOSIMASK) != 0x00)
        {
            byte = byte | 0x01;
        }
        //byte = byte | (PINB & MOSIMASK);
        
        while (clk != 0x00)
        {
            clk = PINB & SCKMASK;
        }
    }
     
    transmitString((unsigned char *)"read cs data: ");
    transmitHex(CHAR, byte);
    transmitString((unsigned char*)"\r\n");
    
    _delay_loop_2(65535);
    _delay_loop_2(65535);
    _delay_loop_2(65535);
    _delay_loop_2(65535);
}
