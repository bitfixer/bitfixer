/*
 * upserial.c
 * userport serial adapter
 *
 * Created: 3/15/2016
 *  Author: Michael Hill
 */

#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "spi_data_protocol.h"

void output_byte(unsigned char byte)
{
    PORTC = byte;
}

unsigned char input_byte()
{
    return PINC;
}

void set_data_output()
{
    DDRC = 0xFF;
}

void set_data_input()
{
    DDRC = 0x00;
}

void init()
{
    DDRA = (1<<PA2);
    PORTA = (1<<PA2);
    // PA0 is PA2 from user port
    // PA1 is /PC2 handshake line
    set_data_input();
}

void lower_flag()
{
    PORTA = 0x00;
}

void raise_flag()
{
    PORTA = (1<<PA2);
}

unsigned char get_command()
{
    // wait for PA0 low
    unsigned char val;
    unsigned char byte;
    do {
        val = PINA & (1<<PA0);
    } while (val != 0);

    // read byte and lower flag
    byte = input_byte();
    lower_flag();

    // wait for PA0 high
    do {
        val = PINA & (1<<PA0);
    } while (val == 0);

    raise_flag();
    return byte;
}

int main(void)
{
    unsigned char val;
    unsigned char cmd;
    int recv_size;
    unsigned char buffer[2048];
    PORTB = 0x01;

    memset(buffer, 0, 2048);

    SPIData spi_data(&PORTB, &DDRB, PB0);
    init();
    spi_data.spi_init();

    // wait for command byte
    while (1)
    {
        cmd = get_command();

        // switch to output mode
        set_data_output();

        // wait for PA0 low
        do {
            val = PINA & (1<<PA0);
        } while (val != 0);

        buffer[0] = cmd;
        recv_size = spi_data.sendAndRecvPacket(buffer, 1);

        PORTB = 0x01;
        PORTD = 0x80;
        // lower flag to indicate ready
        lower_flag();

        // do the transfer
        // clock on PA1 pin, handshake line from user port
        for (int i = 0; i < recv_size; i++)
        {
            output_byte(buffer[i]);
            // wait for handshake pulse to end
            do {
                val = PINA & (1<<PA1);
            } while (val == 0);

            // wait for handshake pulse to start
            do {
                val = PINA & (1<<PA1);
            } while (val != 0);
        }
        output_byte(0x00);
        set_data_input();

        // raise flag, done
        raise_flag();

        // wait for PA0 high, master is done
        do {
            val = PINA & (1<<PA0);
        } while (val == 0);
    }
}