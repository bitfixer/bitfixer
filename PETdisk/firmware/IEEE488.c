/*
    IEEE488.c
    IEEE Routines in the PETdisk storage device
    Copyright (C) 2011 Michael Hill

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


#include "IEEE488.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/signal.h>
#include <inttypes.h>
#include <util/delay.h>
#include <string.h>
#include "UART_routines.h"

void wait_for_dav_high()
{
    unsigned char rdchar;
    rdchar = PINC;
    rdchar = rdchar & DAV;
    while (rdchar == 0x00)
    {
        rdchar = PINC;
        rdchar = rdchar & DAV;
    }
}

void wait_for_dav_low()
{
    unsigned char rdchar;
    rdchar = PINC;
    rdchar = rdchar & DAV;
    while (rdchar != 0x00)
    {
        rdchar = PINC;
        rdchar = rdchar & DAV;
    }
}

void wait_for_atn_high()
{
    unsigned char rdchar;
    rdchar = PINC;
    rdchar = rdchar & ATN;
    while (rdchar == 0x00)
    {
        rdchar = PINC;
        rdchar = rdchar & ATN;
    }
}

void wait_for_atn_low()
{
    unsigned char rdchar;
    rdchar = PINC;
    rdchar = rdchar & ATN;
    while (rdchar != 0x00)
    {
        rdchar = PINC;
        rdchar = rdchar & ATN;
    }
}



void wait_for_nrfd_high()
{
    unsigned char rdchar;
    rdchar = PINC;
    rdchar = rdchar & NRFD;
    while (rdchar == 0x00)
    {
        rdchar = PINC;
        rdchar = rdchar & NRFD;
    }
}

void wait_for_ndac_high()
{
    unsigned char rdchar;
    rdchar = PINC;
    rdchar = rdchar & NDAC;
    while (rdchar == 0x00)
    {
        rdchar = PINC;
        rdchar = rdchar & NDAC;
    }
}

void recv_byte_IEEE(unsigned char *byte)
{
    unsigned char hi, lo;
    hi = DATA_HI_IN;
    lo = DATA_LO_IN;
    
    hi = hi & ~DATALO;
    lo = lo & DATALO;
    
    *byte = ~(hi | lo);
    
}

void send_byte(unsigned char byte, int last)
{
    unsigned char temp,temp2;
    unsigned char lo,hi;
    // invert the byte
    temp = ~byte;
    lo = temp & DATALO;
    hi = temp & ~DATALO;
    
    
    // output the byte
    //PORTA = temp;
    DATA_PORT = hi;
    temp = PORTB;
    temp = temp & ~DATALO;
    temp = temp | lo;
    PORTB = temp;
    
    // wait for NRFD high
    wait_for_nrfd_high();
    
    // lower DAV and EOI
    
    if (last == 0)
    {
        temp = DAV;
        PORTC = ~temp;
    }
    else 
    {
        temp = DAV;
        temp2 = EOI;
        PORTC = (~temp) & (~temp2);
    }

    // wait for NDAC high
    wait_for_ndac_high();
    
    // raise DAV
    temp = DAV | EOI;
    // output to bus
    PORTC = temp;
    
}

void unlisten()
{
    // all bus lines input
    IEEE_CTL = 0x00;
    // set pullups
    IEEE_PORT = 0xff;
}

unsigned char wait_for_device_address(unsigned char my_address)
{
    unsigned char primary_address,dir;
    primary_address = 0;
    //transmitString("waiting..");
    while (primary_address != my_address)
    {
        
        // wait for atn signal
        wait_for_atn_low();
        
        // lower NDAC to respond
        IEEE_PORT = NOT_NDAC;
        // set output lines
        IEEE_CTL = NDAC | NRFD;
        
        // wait for primary address
        wait_for_dav_low();
        
        // lower NRFD
        IEEE_PORT = NOT_NDAC & NOT_NRFD;
        
        // read data
        recv_byte_IEEE(&primary_address);
        dir = primary_address & 0xf0;
        primary_address = primary_address & 0x0f;
        transmitHex(CHAR, primary_address);
        
        if (primary_address == my_address && (dir == TALK || dir == LISTEN))
        {
            // raise NDAC
            IEEE_PORT = NOT_NRFD;
            // wait for data to finish
            wait_for_dav_high();
            
            // release control and handle transaction
            IEEE_PORT = NOT_NDAC;
        }
        else 
        {
            // unlisten the bus
            unlisten();
            // wait for atn to release
            wait_for_atn_high();
        }
    }
    return dir;
}

/*
// added function to generate directory file readable by PET
void generate_directory()
{
    
}
*/
