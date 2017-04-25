//
//  ThreeWireSPI.cpp
//
//
//  Created by Michael Hill on 4/8/17.
//
//

extern "C"
{
#include <avr/io.h>
#include "UART_routines.h"
}

#include "3WireSPI.h"
#include <util/delay.h>

#define CSMASK  0x04
#define SCKMASK 0x20
#define MOSIMASK 0x10
#define MISOMASK 0x08
#define NOTMISOMASK 0xF7

#define TW_SENDCMD 0x01
#define TW_RECVCMD 0x02

void ThreeWireSPI::init()
{
    // if driving the chip select line, enable it
    transmitString((unsigned char*)"ThreeWireSPI init cs\r\n");
    //DDRB = CSMASK | MISOMASK; // data line output
    setOutput();
    PORTB = CSMASK;
}

void ThreeWireSPI::setOutput()
{
    DDRB = DDRB | (CSMASK | MISOMASK);
}

void ThreeWireSPI::setInput()
{
    // set data line input
    DDRB = DDRB & ~MISOMASK;
}

void ThreeWireSPI::setMiso(unsigned char bit)
{
    unsigned char c;
    c = PORTB;
    if (bit == 0)
    {
        c = c & ~MISOMASK;
    }
    else
    {
        c = c | MISOMASK;
    }
    PORTB = c;
}

void ThreeWireSPI::sendCommand(unsigned char cmd, int size)
{
    // chip select is lowered
    unsigned char buf[3];
    buf[0] = cmd;
    buf[1] = (size & 0xFF00) >> 8;
    buf[2] = size & 0x00FF;

    sendBytes(buf, 3);
}

void ThreeWireSPI::sendBytes(unsigned char* bytes, int size)
{
    unsigned char clk = PINB & SCKMASK;
    for (int b = 0; b < size; b++)
    {
        unsigned char sendbyte = bytes[b];
        for (int bit = 0; bit < 8; bit++)
        {
            // present bit
            setMiso(sendbyte & 0x80);
            sendbyte <<= 1;

            while (clk == 0x00)
            {
                clk = PINB & SCKMASK;
            }

            // wait for clock to go low
            while (clk != 0x00)
            {
                clk = PINB & SCKMASK;
            }
        }
    }
}

void ThreeWireSPI::recvBytes(unsigned char* bytes, int size)
{
    unsigned char clk = PINB & SCKMASK;
    for (int b = 0; b < size; b++)
    {
        unsigned char recvbyte = 0;
        for (int bit = 0; bit < 8; bit++)
        {
            while (clk == 0x00)
            {
                clk = PINB & SCKMASK;
            }

            // shift current bit in to receive byte
            recvbyte <<= 1;
            if ((PINB & MISOMASK) != 0x00)
            {
                recvbyte = recvbyte | 0x01;
            }

            // wait for clock to go low
            while (clk != 0x00)
            {
                clk = PINB & SCKMASK;
            }
        }
        bytes[b] = recvbyte;
    }
}

void ThreeWireSPI::waitClockCycle()
{
    unsigned char clk = PINB & SCKMASK;
    while (clk == 0x00)
    {
        clk = PINB & SCKMASK;
    }

    // wait for clock to go low
    while (clk != 0x00)
    {
        clk = PINB & SCKMASK;
    }
}

int ThreeWireSPI::send(unsigned char* buffer, int size)
{
    // lower chip select
    PORTB = 0x00;

    sendCommand(TW_SENDCMD, size);

    // now send actual data
    sendBytes(buffer, size);

    // raise chip select
    PORTB = CSMASK;

    // wait for clock to indicate server done
    waitClockCycle();
}

int ThreeWireSPI::receive(unsigned char* buffer, int size)
{
    // lower chip select
    PORTB = 0x00;

    sendCommand(TW_RECVCMD, size);
    setInput();
    // receive data
    recvBytes(buffer, size);

    PORTB = CSMASK;
    waitClockCycle();
    setOutput();
}

/*
int ThreeWireSPI::transfer(unsigned char* buffer, int size)
{
    unsigned char c = 1;
    unsigned char clk;
    unsigned char cs;
    unsigned char bits = 0;
    unsigned char byte = 0;
    unsigned char sendbyte;
    int bytesReceived = 0;

    if (_driveChipSelect)
    {
        //transmitString((unsigned char*)"cs\r\n");
        // lower chip select line
        PORTB = 0x00;
        c = 0;
    }
    else // read chip select from master
    {
        while (c != 0)
        {
            c = PINB & CSMASK;
        }
    }

    // C version
    clk = PINB & SCKMASK;

    for (int s = 0; s < size && c == 0; s++)
    {
        byte = 0;
        // get current byte in send buffer
        sendbyte = buffer[s];
        for (unsigned char i = 0; i < 8; i++)
        {
            // present bit
            setMiso(sendbyte & 0x80);
            sendbyte <<= 1;

            while (clk == 0x00 && c == 0)
            {
                clk = PINB & SCKMASK;
                if (!_driveChipSelect)
                {
                    c = PINB & CSMASK;
                }
            }

            if (c != 0)
            {
                break;
            }

            // shift current bit in to receive byte
            byte <<= 1;
            if ((PINB & MOSIMASK) != 0x00)
            {
                byte = byte | 0x01;
            }

            // wait for clock to go low
            while (clk != 0x00)
            {
                clk = PINB & SCKMASK;
            }
        }

        if (c == 0)
        {
            // replace byte in buffer with received value
            buffer[s] = byte;
            bytesReceived++;
        }
    }

    if (_driveChipSelect)
    {
        PORTB = CSMASK;
    }

    return bytesReceived;
}
*/
