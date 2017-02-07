/*
    audioserial.c
    Copyright (C) 2016 Michael Hill

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    Contact the author at bitfixer@bitfixer.com
    http://bitfixer.com
*/

#define F_CPU 8000000UL		//freq 8 MHz
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#include <avr/io.h>
#include <util/delay.h>
#include "UART_routines.h"

int main(void)
{
    uart0_init(MYUBRR);
    transmitString("Hello World! 1234567\r\n");
    
    // wait to receive serial packet
    
    DDRB = 0x00;
    DDRC = 0x01;
    
    unsigned char test = 1;
    
    unsigned char val = 0x00;
    unsigned char bits[8];
    unsigned char read_byte;
    int bit;
    int trials;
    int bytes_read = 0;
    unsigned char message[256];
    while (bytes_read < 1)
    {
        val = PINB;
        val = val & 0x01;
        PORTC = val;
        
        if (val == 0x01)
        {
            // need timing for bits
            // wait for one bit duration
            _delay_us(41.67);
            //_delay_loop_1(55);
            //PORTC = 0x00;
            
            //transmitString("Got a bit!\n");
            
            //test = 0x00;
            PORTC = 0x00;
            // get all the bits
            for (bit = 0; bit < 8; bit++)
            {
                // sample from the center of the bit
                
                /*
                bits[bit] = 0;
                _delay_us(20);
                for (trials = 0; trials < 3; trials++)
                {
                    val = PINB;
                    val = val & 0x01;
                    bits[bit] += val;
                }
                _delay_us(17);
                */
                 
                //_delay_us(21.67);
                
                _delay_us(20);
                PORTC = 0x01;
                //val = PINB;
                //val = val & 0x01;
                val = 0x01;
                
                //transmitHex(val, CHAR);
                
                bits[bit] = val;
                _delay_us(21.67);
                PORTC = 0x00;
                
                /*
                bits[bit] = 0;
                for (trials = 0; trials < 10; trials++)
                {
                    _delay_us(2.467);
                    val = PINB;
                    val = val & 0x01;
                    bits[bit] += val;
                }
                */
                //_delay_us(1.67);
                //PORTC = test;
            }
            
            PORTC = 0x01;
            read_byte = 0;
            for (bit = 7; bit >= 0; bit--)
            {
                //transmitHex(bits[bit], CHAR);
                //transmitString("\r\n");
                read_byte = read_byte << 1;
                read_byte = read_byte | bits[bit];
                //if (bits[bit] >= 1)
                //    read_byte = read_byte | 0x01;
            }
            
            message[bytes_read] = read_byte;
            bytes_read = bytes_read + 1;
            
            PORTC = 0x00;
            
            //transmitString("TADA: received string!!!!!\n");
            //transmitByte(read_byte);
            //transmitString("\r\n that was it\r\n");
            
            /*
            val = PINB;
            val = val & 0x01;
            while (val != 0x00)
            {
                val = PINB;
                val = val & 0x01;
            }
            */
            
            // wait for a zero
            // stop bit
            _delay_us(20);
        }
        
        
        /*
        PORTC = test;
        if (test == 1)
            test = 0;
        else
            test = 1;
        */
        
        
        
        //transmitHex(val, CHAR);
        //transmitString("\r\n");
        
        /*
        int i;
        if (val == 0x01)
        {
            
            int found = 1;
            
            if (found == 1)
            {
                transmitString("Got 1 bit!\r\n");
            }
        }
        
        /*
        if (val == 0x01)
        {
            transmitString("Got a 1!\r\n");
        }
        else
        {
            transmitString("Got a 0!\r\n");
        }
        */
    }
    
    transmitString("HEEHEHEHEHEH\r\n");
    
    int b;
    for (b = 0; b < bytes_read; b++)
    {
        //transmitByte('a');
        //transmitByte(message[b]);
        transmitHex(message[b], CHAR);
        transmitString("\n");
    }
    
}