//
//  spi_data_protocol.h
//
//
//  Created by Michael Hill on 8/8/17.
//
//

#ifndef spi_data_protocol_h
#define spi_data_protocol_h

#include <stdio.h>

class SPIData
{
public:
    SPIData(volatile unsigned char* readyOutPort,
            volatile unsigned char* readyReg,
            int readyPin)
    : _readyOutPort(readyOutPort)
    , _readyReg(readyReg)
    , _readyPinMask(1<<readyPin)
    {};

    ~SPIData() {};
    
    int sendAndRecvPacket(unsigned char* data, int send_size);
    void spi_init();

private:
    volatile unsigned char* _readyOutPort;
    volatile unsigned char* _readyReg;
    unsigned char _readyPinMask;

    unsigned char spi_receive(void);
    unsigned char spi_transmit(unsigned char data);
};

#endif /* spi_data_protocol_h */
