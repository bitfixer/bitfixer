//
//  spi_data_protocol.cpp
//
//
//  Created by Michael Hill on 8/8/17.
//
//

#include "spi_data_protocol.h"
#include <avr/io.h>

//#define USE_READY 1

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
    reg = *_spiReg;
    reg |= _spiMisoMask;
    *_spiReg = reg;

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

    #ifdef USE_READY
        *_readyOutPort = port;
    #else
        // send ready byte
        tmp = 0xFF;
        // clear SPI flag by sending FF (same as no response)
        spi_transmit(tmp);

        // send 3 byes 0xA5, align receiver
        tmp = 0xA5;
        spi_transmit(tmp);
        spi_transmit(tmp);
        spi_transmit(tmp);

        // send 1 byte 0x5A, signal start of real data
        tmp = 0x5A;
        spi_transmit(tmp);
    #endif

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

    #ifdef USE_READY
        // raise ready line
        *_readyOutPort = port;
    #endif

    // wait for one byte for sync
    tmp = spi_receive();
    return recv_size;
}
