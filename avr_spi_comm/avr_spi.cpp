/*
 * upserial.c
 * PET user port spi adapter
 *
 * Created: 3/15/2016
 *  Author: Michael Hill
 */

#include "spi_data_protocol.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define SPI_READY_REG       DDRB
#define SPI_READY_OUTPORT   PORTB
#define SPI_READY_PIN       PB0

#define BUFFER_SIZE 1024

int main(void)
{
    unsigned char val;
    unsigned char cmd;
    int packet_size;
    unsigned char buffer[BUFFER_SIZE];

    SPIData spi_data(&SPI_READY_OUTPORT, &SPI_READY_REG, SPI_READY_PIN);
    spi_data.spi_init();

    //while (1)
    {
        //_delay_us(1000000);
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            buffer[i] = 'A' + (i%26);
        }
        packet_size = spi_data.sendAndRecvPacket(buffer, 26);
    }

    //_delay_us(1000000);
    spi_data.sendAndRecvPacket(buffer, packet_size);
}
