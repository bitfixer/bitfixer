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
#define RUN_STOP 3

int main (void)
{
    unsigned char *colormem = COLOR_RAM;
    unsigned char *cursorpos = (unsigned char *)VMEM_START;
    int x;
    for (x = 0; x < 1000; x++)
    {
        colormem[x] = 0;
    }
    
    // initialize serial port
    serial_init();
    
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
    asm("sta %w", VMEM_START);
    
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


