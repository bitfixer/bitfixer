/*
    PETdisk.c
    Main program for the PETdisk storage device
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

#define F_CPU 8000000UL		//freq 8 MHz
#define BAUD 38400
#define MYUBRR F_CPU/16/BAUD-1

extern "C"
{
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>
#include "UART_routines.h"
}
#include "softSpi.h"

int main(void)
{
    softSpi spi(false // drivechipSelect
                );
    spi.init();

    uart0_init(MYUBRR);
    transmitString((unsigned char*)"starting\r\n");

    unsigned char buffer[600];
    int bytesReceived = 0;
    while (1)
    {
        for (int b = 0; b < 512; b++)
        {
            buffer[b] = (unsigned char)b;
        }

        bytesReceived = spi.transfer_asm(buffer, 512);

        transmitHex(INT, bytesReceived);
        transmitString((unsigned char*)"\r\n");

        for (int b = 0; b < 512; b++)
        {
            transmitByte(buffer[b]);
        }
        transmitString((unsigned char*)"\r\n");
    }
}
