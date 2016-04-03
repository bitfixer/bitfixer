/**********

client.c
userport serial clients
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

// memory addresses for I/O
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
    
    // set portb (userport data) to input
    POKE(DDRB, 0x00);
    
    // set handshake line PA2 to output
    POKE(DDRA, 0x04);
    
    // set PA2 high
    POKE(PORTA, 0x04);
    
    // read ICR to clear flag
    data = PEEK(ICR);
    
    while (!done)
    {
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
         
        POKE(PORTA, 0x04);
    }
    
	return EXIT_SUCCESS;
}


