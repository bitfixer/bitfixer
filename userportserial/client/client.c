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
#include "serial.h"

#define VMEM_START      0x0400
#define SIDADDR         0xD400
#define RUN_STOP 3

typedef struct
{
    unsigned char buffer[3];
    unsigned char *type;
    unsigned char *sid_offset;
    unsigned char *value;
    unsigned char bytes_recv;
    unsigned char ready;
} control_packet;

void init_control_packet(control_packet *pkt)
{
    pkt->type = &pkt->buffer[0];
    pkt->sid_offset = &pkt->buffer[1];
    pkt->value = &pkt->buffer[2];
    
    *pkt->type = 0;
    *pkt->sid_offset = 0;
    *pkt->value = 0;
    pkt->bytes_recv = 0;
    pkt->ready = 0;
}

void add_byte_to_control_packet(unsigned char byte, control_packet *pkt)
{
    pkt->buffer[pkt->bytes_recv] = byte;
    pkt->bytes_recv++;
    
    if (pkt->bytes_recv >= 3)
    {
        pkt->bytes_recv = 0;
        pkt->ready = 1;
    }
}

void process_control_packet(control_packet *pkt)
{
    // process packet according to type
    if (*pkt->type == 1)
    {
        unsigned char *sid = (unsigned char *)SIDADDR;
        sid[*pkt->sid_offset] = *pkt->value;
    }
}

int main (void)
{
    unsigned char data;
    unsigned char done = 0;
    unsigned char spinner = 0;
    unsigned char *cursorpos = (unsigned char *)VMEM_START;
    unsigned char *colormem = COLOR_RAM;
    int x;
    
    control_packet pkt;
    init_control_packet(&pkt);
    
    data = test();
    
    for (x = 0; x < 1000; x++)
    {
        colormem[x] = 0;
    }
    
    serial_init();
    
    while (!done)
    {
        // check for a ready control packet
        if (pkt.ready)
        {
            process_control_packet(&pkt);
            pkt.ready = 0; // packet no longer ready
        }
        
        if (serial_byte_ready())
        {
            // process byte
            data = serial_read_byte();
            
            // do stuff
            if ((data == 'q' || data == 'Q') && pkt.bytes_recv == 0)
            {
                done = 1;
            }
            else
            {
                add_byte_to_control_packet(data, &pkt);
                *cursorpos = data;
                cursorpos++;
            }
            
            serial_done_reading();
        }
        else
        {
            // idle, do background tasks
            *cursorpos = spinner++;
        }
    }
    
    
    /*
    while (!done)
    {
        if (serial_byte_ready())
        {
            data = serial_read_byte();
            *cursorpos = data;
            cursorpos++;
            if (data == 'q' || data == 'Q')
            {
                done = 1;
            }
            serial_done_reading();
        }
        else
        {
            *cursorpos = spinner++;
        }
    }
    */
    
    
	return EXIT_SUCCESS;
}


