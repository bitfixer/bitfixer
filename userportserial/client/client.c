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
#define CIA2_ICR        0xDD0D
#define CIA2_PRA        0xDD00
#define CIA2_PRB        0xDD01

#define BGCOLOR         0xD020
#define BORDERCOLOR     0xD021

#define RUN_STOP 3

extern unsigned char data[];

int main (void)
{
    unsigned char *colormem = COLOR_RAM;
    unsigned char *cursorpos = (unsigned char *)VMEM_START;
    int x;
    int y;
    int z;
    for (x = 0; x < 1000; x++)
    {
        colormem[x] = 6;
    }
    
    // initialize serial port
    serial_init();
    
    // set colors
    POKE(BGCOLOR, 0);
    POKE(BORDERCOLOR, 0);
    
    for (x = 0; x < 1000; x++)
    {
        cursorpos[x] = data[x];
    }
    
    for (y = 0; y < 4; y++)
    {
        z = 503 + y*40;
        for (x = 0; x < 15; x++)
        {
            colormem[z + x] = 2;
        }
    }
    
    for (x = 0; x < 40; x++)
    {
        //cursorpos[x] = '*';
        colormem[x] = (x % 15)+1;
        colormem[960+x] = (x % 15)+1;
    }
    
    z = 0;
    
    //asm("ldx #$00");
loopstart:
    // read one byte
    asm("lda %w", CIA2_ICR);
    asm("and #$10"); // and the flag bit in ICR
    asm("beq %g", loopstart);
    
firstbyte:
    
    // read byte from port B
    asm("lda %w", CIA2_PRB);
    
    // store value in x
    asm("tax");
    
    
    // poke value to video memory
    //asm("sta %w", VMEM_START);
    //asm("sta %w,x", VMEM_START);
    //asm("inx");
    
    asm("ldy #$00");
    asm("tya");
    asm("sta %w", CIA2_PRA);
    asm("ldy #$04");
    asm("tya");
    
    // some nops for delay
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    
    asm("sta %w", CIA2_PRA);
    //asm("jmp %g", loopstart);
    

loopstart2:
    // read one byte
    asm("lda %w", CIA2_ICR);
    asm("and #$10"); // and the flag bit in ICR
    asm("beq %g", loopstart2);
    
secondbyte:
    
    // read byte from port B
    asm("lda %w", CIA2_PRB);
    
    // store value in offset determined by x
    asm("sta %w,x", SIDADDR);
    
    asm("ldy #$00");
    asm("tya");
    asm("sta %w", CIA2_PRA);
    asm("ldy #$04");
    asm("tya");
    
    // some nops for delay
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    
    asm("sta %w", CIA2_PRA);
    
    //asm("jmp %g", loopstart);
    
    
    cursorpos[z] = 0x20;
    cursorpos[999-z] = 0x20;
    z++;
    if (z >= 40)
        z = 0;
    cursorpos[z] = '*';
    cursorpos[999-z] = '*';
    asm("jmp %g", loopstart);
    
    /*
    asm("ldy #$00");
    
    // read byte from port B
    asm("lda %w", CIA2_PRB);
    
    // poke value to video memory
    asm("sta %w", VMEM_START);
    
    // store value in x register
    asm("tax");
    
    // transfer y to accumulator
    asm("tya");
    asm("sta %w", CIA2_PRA);
    
    // PA2 bit for signalling
    asm("ldy #$04");
    // transfer y to accumulator
    asm("tya");
    
    // some nops for delay
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    
    asm("sta %w", CIA2_PRA);
    
    asm("jmp %g", loopstart);
    */
    
/*

    
secondbyte:
    
    asm("ldy #$00");
    // read byte from port B
    asm("lda %w", CIA2_PRB);
    
    // store value in offset determined by x
    asm("sta %w,x", SIDADDR);
    
    // transfer y to accumulator
    asm("tya");
    asm("sta %w", CIA2_PRA);
    
    // PA2 bit for signalling
    asm("ldy #$04");
    // transfer x to accumulator
    asm("tya");
    
    // some nops for delay
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    
    asm("sta %w", CIA2_PRA);
    asm("jmp %g", loopstart);
*/
    
    // read another byte
    
    
    // poke value
    
    
    // do it again
    
    
	return EXIT_SUCCESS;
}


