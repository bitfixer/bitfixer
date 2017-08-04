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
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "SPI_routines.h"
#include "SD_routines.h"
#include "UART_routines.h"

#define SPI_PORT PORTB
#define SPI_CTL  DDRB
#define MISO     0x10

#define SOH 0x01
#define ACK 0x06
#define EOT 0x04
#define NAK 0x15
#define XEOF 0x1A

//use following macros if PB1 pin is used for Chip Select of SD
//#define SD_CS_ASSERT     PORTB &= ~0x04
//#define SD_CS_DEASSERT   PORTB |= 0x04

void port_init(void)
{
    SPI_CTL = (unsigned char)(~MISO);
    SPI_PORT = 0x7f;
}

//call this routine to initialize all peripherals
void init_devices(void)
{
 cli();  //all interrupts disabled
 port_init();
 spi_init();
 uart0_init(MYUBRR);
 MCUCR = 0x00;
}

uint16_t calcrc(unsigned char *ptr, int count)
{
    int crc;
    char i;
    
    crc = 0;
    while (--count >= 0)
    {
        crc = crc ^ (int) *ptr++ << 8;
        i = 8;
        do
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
        } while(--i);
    }
    return (crc);
}

// send bytes over SPI with data checking
void send_data(unsigned char *data, int numbytes)
{
    int bytes_sent = 0;
    int16_t crc = 0;
    int chunksize = 16;
    unsigned char temp = 0;
    unsigned char pktnum = 0;
    unsigned char pkt[65];
    
    while (bytes_sent < numbytes)
    {
        pkt[0] = pktnum;
        for (int i = 0; i < chunksize; i++)
        {
            pkt[i+1] = data[bytes_sent+i];
        }
        
        crc = calcrc(&pkt, chunksize+1);
        SD_CS_ASSERT;
        
        // send packet number
        for (int i = 0; i < chunksize+1; i++)
        {
            SPI_transmit(pkt[i]);
            //_delay_ms(1);
        }
        
        temp = (crc >> 8) & 0xFF;
        SPI_transmit(temp);
        //_delay_ms(1);
        
        temp = crc & 0xFF;
        SPI_transmit(temp);
        SD_CS_DEASSERT;
        
        // get 1 byte reply
        _delay_ms(1);
        SD_CS_ASSERT;
        _delay_ms(1);
        temp = SPI_receive();
        _delay_ms(1);
        
        // check for ACK, indicating successful receive
        if (temp == ACK)
        {
            bytes_sent += chunksize;
            pktnum++;
            
            SPI_transmit(ACK);
        }
        else
        {
            SPI_transmit(NAK);
        }
        
        SD_CS_DEASSERT;
        _delay_ms(1);
        
    }
    
    
}


int main(void)
{
    unsigned char b;
    init_devices();
    
    _delay_ms(100);
    
    for (int i = 0; i < 512; i++)
    {
        _buffer[i] = i;
    }
    
    send_data(_buffer, 512);
    
    /*
    int count = 0;
    int chunksize = 64;
    while (count < 512)
    {
        int16_t crc = calcrc(&_buffer[count], chunksize);
        
        SD_CS_ASSERT;
        for (int i = count; i < count + chunksize; i++)
        {
            SPI_transmit(_buffer[i]);
            _delay_ms(1);
        }
        
        // send crc
        b = (crc >> 8) & 0xFF;
        SPI_transmit(b);
        _delay_ms(1);
        
        b = crc & 0xFF;
        SPI_transmit(b);
        SD_CS_DEASSERT;
        
        
        _delay_ms(1);
        SD_CS_ASSERT;
        _delay_ms(1);
        b = SPI_receive();
        _delay_ms(1);
        if (b == ACK)
        {
            count += chunksize;
            SPI_transmit(ACK);
        }
        else
        {
            SPI_transmit(NAK);
        }
        
        SD_CS_DEASSERT;
        _delay_ms(1);
    }
    */
    //SD_writeSingleBlock(0x01);
    
    while (1)
    {
    }
}