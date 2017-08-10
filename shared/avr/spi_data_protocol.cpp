//
//  spi_data_protocol.cpp
//
//
//  Created by Michael Hill on 8/8/17.
//
//

#include "spi_data_protocol.h"
#include <avr/io.h>

#define DDR_SPI     DDRB
#define DD_MISO     DDB6

unsigned char SPIData::spi_receive(void)
{
    unsigned char data;
    // Wait for reception complete

    SPDR = 0xff;
    while(!(SPSR & (1<<SPIF)));
    data = SPDR;

    // Return data register
    return data;
}

unsigned char SPIData::spi_transmit(unsigned char data)
{
    // Start transmission
    SPDR = data;

    // Wait for transmission complete
    while(!(SPSR & (1<<SPIF)));
    data = SPDR;

    return(data);
}

void SPIData::spi_init()
{
    // initialize registers
    unsigned char reg = *_readyReg;
    reg |= _readyPinMask;
    *_readyReg = reg;

    // enable MISO as output
    reg = DDR_SPI;
    reg |= 1<<DD_MISO;
    DDR_SPI = reg;

    unsigned char port = *_readyOutPort;
    port |= _readyPinMask;
    *_readyOutPort = port;

    SPCR = (1<<SPE); // slave, fosc/4 (2MHz)
    SPSR = 0x00;
}

int SPIData::sendAndRecvPacket(unsigned char* data, int send_size)
{
    // lower ready line
    unsigned char tmp;
    int recv_size;
    unsigned char size_bytes[2];
    unsigned char port = *_readyOutPort;
    port &= ~_readyPinMask;

    // send size of send packet
    size_bytes[0] = (send_size & 0xFF00) >> 8;
    size_bytes[1] = send_size & 0x00FF;

    /*
    // send ready byte
    tmp = 0x12;
    spi_transmit(tmp);
    spi_transmit(tmp);
    spi_transmit(tmp);
    tmp = 0;
    spi_transmit(tmp);
    */

    *_readyOutPort = port;
    spi_transmit(size_bytes[0]);
    spi_transmit(size_bytes[1]);

    for (int i = 0; i < send_size; i++)
    {
        spi_transmit(data[i]);
    }

    size_bytes[0] = spi_receive();
    size_bytes[1] = spi_receive();
    recv_size = (size_bytes[0] << 8) + size_bytes[1];
    for (int i = 0; i < recv_size; i++)
    {
        data[i] = spi_receive();
    }

    port |= _readyPinMask;
    // raise ready line
    *_readyOutPort = port;

    // wait for one byte for sync
    tmp = spi_receive();
    return recv_size;
}
