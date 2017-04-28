/*
    UART_routines.c
    Serial/UART Routines in the PETdisk storage device
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

#include "UART_routines.h"
#include <avr/io.h>
#include <avr/pgmspace.h>

//**************************************************
//UART0 initialize
//baud rate: 19200  (for controller clock = 8MHz)
//char size: 8 bit
//parity: Disabled
//**************************************************
void uart0_init(unsigned int ubrr)
{
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;

    UCSR0A = 0x00;
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

//**************************************************
//Function to receive a single byte
//*************************************************
unsigned char receiveByte(void)
{
	unsigned char data;
	while(!(UCSR0A & (1<<RXC0))); 	// Wait for incoming data
	data = UDR0;
	return data;
}
//***************************************************
//Function to transmit a single byte
//***************************************************
void transmitByte(unsigned char data)
{
    while ( !(UCSR0A & (1<<UDRE0)) );    // Wait for empty transmit buffer
	UDR0 = data;                         // Start transmission
}

//***************************************************
//Function to transmit hex format data
//first argument indicates type: CHAR, INT or LONG
//Second argument is the data to be displayed
//***************************************************

void transmitHex( unsigned char dataType, unsigned long data )
{
    unsigned char count, i, temp;
    unsigned char dataString[] = "0x        ";

    if (dataType == CHAR) count = 2;
    if (dataType == INT) count = 4;
    if (dataType == LONG) count = 8;

    for(i = count; i > 0; i--)
    {
        temp = data % 16;
        if((temp>=0) && (temp<10)) dataString [i+1] = temp + 0x30;
        else dataString [i+1] = (temp - 10) + 0x41;

        data = data/16;
    }
    transmitString_nonewline (dataString);
}

//***************************************************
//Function to transmit a string in Flash
//***************************************************
void transmitString_F(const unsigned char* string)
{
    while (pgm_read_byte(&(*string)))
    {
        transmitByte(pgm_read_byte(&(*string++)));
    }

    TX_NEWLINE;
}

//***************************************************
//Function to transmit a string in RAM
//***************************************************
void transmitString(const unsigned char* string)
{
    while (*string)
    {
        transmitByte(*string++);
    }

    TX_NEWLINE;
}

void transmitString_nonewline(const unsigned char* string)
{
    while (*string)
    {
        transmitByte(*string++);
    }
}
