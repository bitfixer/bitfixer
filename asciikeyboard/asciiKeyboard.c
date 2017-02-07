/*
    asciiKeyboard.c
    Copyright (C) 2017 Michael Hill

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
#define BAUD 19200
#define MYUBRR F_CPU/16/BAUD-1
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>
#include "UART_routines.h"

typedef enum { false, true } bool;

//call this routine to initialize all peripherals
void init_devices(void)
{
    uart0_init(MYUBRR);
}

int main(void)
{
    DDRD = 0x02;
    PORTD = 0xFF;

    DDRA = 0xFF;
    PORTA = 0x00;

    init_devices();

    unsigned char c;
    unsigned char byte;
    while (1)
    {
        c = receiveByte();
        byte = c;
        byte = byte | 0x80;   // raise MSB, for pulse

        PORTA = byte;      // put character on the lines
        _delay_loop_2(65535);
        byte = byte & 0x7F;   // lower MSB, pulse finished
        PORTA = byte;
        transmitByte(c);
        //
        //transmitByte(c);
    }

    return 1;
}
