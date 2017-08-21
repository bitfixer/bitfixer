/*
    c64drive.cpp
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

extern "C" {
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>
}

#define SPI_PORT PORTB
#define SPI_CTL  DDRB
#define MISO     0x10

#define C64_DRIVE_ID    0x12

unsigned char* gBuffer;

ISR(USI_OVF_vect)
{
    PORTB = 0x00;
    USISR = (1<<USIOIF);
    *gBuffer = USIBR;
    gBuffer++;
    PORTB = 0x01;
}

void usi_setup()
{
    unsigned char reg;
    reg = DDRA;
    reg = reg | (1<<PA5);
    DDRA = reg;
    reg = (1<<USIWM0) | (1<<USICS1);
    
    // enable interrupt
    //reg = (1<<USIWM0) | (1<<USICS1) | (1<<USIOIE);
    USICR = reg;
}

void send_usi_byte(unsigned char byte)
{
    unsigned char reg;
    USIDR = byte;
    reg = (1<<USIOIF);
    USISR = reg;
    
    while ((USISR & (1<<USIOIF)) == 0);
}

unsigned char recv_usi_byte()
{
    USISR = (1<<USIOIF);
    while ((USISR & (1<<USIOIF)) == 0);
    
    /*
    asm("wait_loop:" "\r\n"
        "in r16, %0" "\r\n"
        "sbrs r16, %1" "\r\n"
        "rjmp wait_loop" "\r\n"
        "ldi r16, (1<<%1)" "\r\n"
        "out %0, r16" "\r\n"
        :
        : "I" (_SFR_IO_ADDR(USISR)), "M" (USIOIF)
        );
    */
    
    return USIBR;
}

void spi_trans(unsigned char* bytes, int size)
{
    unsigned char reg, reg2, byte;
    unsigned char* ptr = bytes;
    int rsize;
    reg2 = DDRA;
    reg = DDRA | (1<<PA5);
    
    PORTA = 0x00;
    while ((PINA & 0x02) != 0);
    DDRA = reg;
    send_usi_byte(0);
    send_usi_byte(1);
    send_usi_byte('Q');
    
    gBuffer = bytes;
    
    USISR = (1<<USIOIF);
    // enable interrupts
    sei();
    reg = (1<<USIWM0) | (1<<USICS1) | (1<<USIOIE);
    USICR = reg;
    
    // wait for chip select
    while ((PINA & 0x02) == 0);
    //PORTB = 0x01;
    
    // ISR should handle byte reception
    // wait for deassert
    while ((PINA & 0x02) != 0);
    PORTB = 0x01;
    
    
    // wait for chip select
    while ((PINA & 0x02) == 0);
    PORTB = 0x00;
    //PORTB = 0x00;
    
    while ((PINA & 0x02) != 0);
    while ((PINA & 0x02) == 0);
    
    // disable interrupts
    reg = (1<<USIWM0) | (1<<USICS1);
    USICR = reg;
    PORTA = 0x01;
    
    /*
    PORTB = 0x01;
    
    byte = recv_usi_byte();
    rsize = recv_usi_byte();
    
    PORTB = 0x00;
    
    for (int b = 0; b < 10; b++)
    {
        //byte = recv_usi_byte();
        USISR = (1<<USIOIF);
        while ((USISR & (1<<USIOIF)) == 0);
        *(ptr++) = USIBR;
    }
    
    PORTA = 0x01;
    */
    
    /*
    //send_usi_byte('X');
    //USISR = (1<<USIOIF);
    //while ((USISR & (1<<USIOIF)) == 0);
    byte = recv_usi_byte();
    
    PORTB = 0x01;
    //send_usi_byte('X');
    //USISR = (1<<USIOIF);
    //while ((USISR & (1<<USIOIF)) == 0);
    byte = recv_usi_byte();
    

    PORTB = 0x00;
    //send_usi_byte('X');
    //USISR = (1<<USIOIF);
    //while ((USISR & (1<<USIOIF)) == 0);
    byte = recv_usi_byte();
    PORTB = 0x01;
    */
    
    //send_usi_byte('X');
    byte = recv_usi_byte();
    DDRA = reg2;
}

int main(void)
{
    unsigned char buf[128];
    
    DDRA = 0x01;
    PORTA = 0x01;
    usi_setup();
    sei();
    
    // test
    DDRA = 0x01;
    DDRB = 0xFF;
    _delay_us(1000000);
    //while (1)
    {
        PORTB = 0x00;
        spi_trans(buf, 0);
        PORTB = 0x00;
        _delay_us(1000000);
        /*
        PORTA = 0x00;
        // wait for chip enable
        while ((PINA & 0x02) != 0);
        DDRA = DDRA | (1<<PA5);
        send_usi_byte('A');
        DDRA = DDRA & ~(1<<PA5);
        PORTA = 0x01;
        
        _delay_us(1000000);
        PORTA = 0x00;
        while ((PINA & 0x02) != 0);
        DDRA = DDRA | (1<<PA5);
        send_usi_byte('B');
        DDRA = DDRA & ~(1<<PA5);
        PORTA = 0x01;
        */
    }
    
    while (1)
    {
        
    }
}
