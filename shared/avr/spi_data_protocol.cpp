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

void SPIData::sendAndRecvPacket(unsigned char* data, int size)
{
    /*
    unsigned char port = *_readyOutPort;
    port &= ~_readyPinMask;
    
    SPDR = data[0];
    
    *_readyOutPort = port;
    while (!(SPSR & (1<<SPIF)));
    //data = SPDR;
    
    port |= _readyPinMask;
    *_readyOutPort = port;
    */
    
    // lower ready line
    unsigned char tmp;
    unsigned char port = *_readyOutPort;
    port &= ~_readyPinMask;
    
    /*
    SPDR = 'A';
    *_readyOutPort = port;
    while(!(SPSR & (1<<SPIF)));
    tmp = SPDR;
    
    SPDR = 'B';
    while(!(SPSR & (1<<SPIF)));
    */
    
    *_readyOutPort = port;
    for (int i = 0; i < size; i++)
    {
        SPDR = data[i];
        while(!(SPSR & (1<<SPIF)));
        tmp = SPDR;
    }
    
    for (int i = 0; i < size; i++)
    {
        data[i] = spi_receive();
    }
    
    port |= _readyPinMask;
    *_readyOutPort = port;
}
