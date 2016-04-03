/**********

petpix_sdclient
-----

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2 of the License.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
    
    More information about GNU can be found at the following website:
    http://www.gnu.org/
    
**********/


#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <peekpoke.h> 
#include <time.h>
#include <c64.h>
#include <string.h>


/*
original basic program

5 poke 59468,peek(59468) or 1
6 poke 59467, peek(59467) and 227
8 poke 59468,peek(59468) or 224
10 poke 59467,peek(59467) and 254
20 a = peek(59457)
30 poke 32768,45
40 if(peek(59469) and 2) > 0 then 60
42 poke 32768,92
44 if(peek(59469) and 2) > 0 then 60
45 poke 32768,93
46 if(peek(59469) and 2) > 0 then 60
47 poke 32768,47
48 if(peek(59469) and 2) > 0 then 60
50 goto 30
60 for x = 1 to 1000 step 2
80 poke 59468,(peek(59468) and 31) or 192
90 a = peek(59457)
100 poke 32767+x,a
110 poke 59468, peek(59468) or 224
120 b = peek(59457)
130 poke 32767+x+1,b
140 next x
150 goto 5
*/

//#define CA1_TRIGGER     59468
//#define CA1_DATA_LATCH  59467
//#define CA1_STATUS      59469
//#define USERPORT_DATA   59457
//#define VMEM_START      32768

//#define CA1_TRIGGER     0xE84C
//#define CA1_DATA_LATCH  0xE84B
//#define CA1_STATUS      0xE84D
//#define USERPORT_DATA   0xE841
//#define VMEM_START      0x8000


#define PORTB           0xDD01
#define DDRB            0xDD03
#define PORTA           0xDD00
#define DDRA            0xDD02
#define ICR             0xDD0D

#define VMEM_START      0x0400
#define COLORMEM_START  0xD800
#define RUN_STOP 3

int main (void)
{
    unsigned char data;
    unsigned char d1;
    unsigned char done = 0;
    unsigned char spinner = 0;
    unsigned char *cursorpos = (unsigned char *)VMEM_START;
    unsigned char *colormem = (unsigned char *)COLORMEM_START;
    unsigned char *icr = (unsigned char *)ICR;
    int x;
    
    for (x = 0; x < 1000; x++)
    {
        colormem[x] = 0;
    }
    
    /*
    // disable flag interrupt
    data = *icr;
    data = data & 0xEF;
    *icr = data;
    */
    
    // set portb (userport data) to input
    POKE(DDRB, 0x00);
    
    // set handshake line PA2 to output
    POKE(DDRA, 0x04);
    
    // set PA2 high
    POKE(PORTA, 0x04);
    
    /*
    data = PEEK(CA1_TRIGGER);
    data = data | 0x01;
    POKE(CA1_TRIGGER, data);
    
    // disable shift register modes
    data = PEEK(CA1_DATA_LATCH);
    data = data & 227;
    POKE(CA1_DATA_LATCH, data);
    
    // set CB2 line high poke 59468,peek(59468) or 224
    data = PEEK(CA1_TRIGGER);
    data = data | 224;
    POKE(CA1_TRIGGER, data);
    
    // data not latched
    data = PEEK(CA1_DATA_LATCH);
    data = data & 254;
    POKE(CA1_DATA_LATCH, data);
    
    // read data, reset CA1 flag bit
    data = PEEK(USERPORT_DATA);
    */
    
    // read ICR to clear flag
    data = PEEK(ICR);
    
    while (!done)
    {
        //data = *icr & 0x10;
        //data = PEEK(PORTB);
        //*cursorpos = data;
        
        
        // look for FLAG2 (data ready)
        data = PEEK(ICR);
        data = data & 0x10;
        while (data == 0)
        {
            *cursorpos = spinner++;
            data = PEEK(ICR);
            data = data & 0x10;
        }
        
        // data is ready, get a byte from userport
        data = PEEK(PORTB);
        
        // display
        *cursorpos = data;
        // toggle PA2 to indicate byte was read
        POKE(PORTA, 0x00);
        cursorpos++;
        
        if (data == 'q' || data == 'Q')
        {
            done = 1;
        }
        
        
        /*
        // delay
        for (x = 0; x < 100; x++)
        {
            *cursorpos = spinner++;
        }
        */
         
        POKE(PORTA, 0x04);
        
        
        /*
        data = data & 0x10;
        
        while (data == 0)
        {
            *cursorpos = spinner++;
            data = PEEK(ICR);
            data = data & 0x10;
        }
        */
        
        
        /*
        // wait for a byte to be ready
        data = PEEK(CA1_STATUS);
        data = data & 2;
        
        while (data != 0)
        {
            *cursorpos = spinner++;
            data = PEEK(CA1_STATUS);
            data = data & 2;
            // *cursorpos = '0'+data;
        }
        
        // read the byte
        data = PEEK(USERPORT_DATA);
        *cursorpos = data;
        cursorpos++;
        
        // set CB2 line low to indicate byte has been read
        // poke 59468,(peek(59468) and 31) or 192
        data = PEEK(CA1_TRIGGER);
        data = (data & 31) | 192;
        POKE(CA1_TRIGGER, data);
        
        // wait for CA1 line low
        data = PEEK(CA1_STATUS);
        data = data & 2;
        while (data == 0)
        {
            data = PEEK(CA1_STATUS);
            data = data & 2;
        }
        
        // set CB2 line high
        data = PEEK(CA1_TRIGGER);
        data = data | 224;
        POKE(CA1_TRIGGER, data);
        */
    }
    
	return EXIT_SUCCESS;
}


