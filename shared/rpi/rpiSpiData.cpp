#include "rpiSpiData.h"
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <unistd.h>
#include <stdio.h>

int rpiSpiData::receive(unsigned char* buffer)
{
    // wait for device to signal ready
    while (digitalRead(_deviceReadyPin) == HIGH)
    {
        delayMicroseconds(10);
    }

    /*
    unsigned char tmp;
    read(_spi, &tmp, 1);
    if (tmp != 0x12)
    {
        return 0;
    }
    else
    {
        while (tmp == 0x12)
        {
            int r = read(_spi, &tmp, 1);
        }
    }
    */

    // receive packet length
    int r = 0;
    unsigned char size_bytes[2];
    read(_spi, &size_bytes[0], 1);
    read(_spi, &size_bytes[1], 1);
    printf("got %X %X\n", size_bytes[0], size_bytes[1]);
    
    int size = (size_bytes[0] << 8) + size_bytes[1];

    for (int i = 0; i < size; i++)
    {
        r += read(_spi, &buffer[i], 1);
    }

    return r;
}

void rpiSpiData::send(unsigned char* buffer, int send_size)
{
    int r = 0;
    unsigned char size_bytes[2];
    size_bytes[0] = (send_size & 0xFF00) >> 8;
    size_bytes[1] = send_size & 0x00FF;

    r = write(_spi, size_bytes, 2);
    r = write(_spi, buffer, send_size);

    // wait for device to deassert ready
    while (digitalRead(_deviceReadyPin) == LOW)
    {
        delayMicroseconds(10);
    }

    // write one byte for sync
    unsigned char tmp = 0xff;
    r = write(_spi, &tmp, 1);
}
