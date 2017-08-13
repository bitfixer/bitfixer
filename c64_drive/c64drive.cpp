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

#include "spi_data_protocol.h"
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

#define UNLISTEN 0x3F

SPIData spi_data(&PORTB, &DDRB, PB0, &DDRB, PB4);

class c64iec
{
public:
    c64iec(volatile unsigned char* iecOutPort,
           volatile unsigned char* iecInPort,
           volatile unsigned char* iecReg,
           int atnPin,
           int clkPin,
           int dataPin)
    : _iecOutPort(iecOutPort)
    , _iecInPort(iecInPort)
    , _iecReg(iecReg)
    , _atnPinMask(1<<atnPin)
    , _clkPinMask(1<<clkPin)
    , _dataPinMask(1<<dataPin)
    {
        
    }
    
    ~c64iec() {};
    
    void init()
    {
    }
    
    void waitForAtn()
    {
        while ((*_iecInPort & _atnPinMask) != 0);
    }
    
    void waitForNotAtn()
    {
        while ((*_iecInPort & _atnPinMask) == 0);
    }
    
    bool atnTrue()
    {
        return ((*_iecInPort & _atnPinMask) == 0);
    }
    
    bool waitForClk(int timeoutUs = -1)
    {
        //while ((*_iecInPort & _clkPinMask) != 0);
        
        int timeUs = 0;
        while ((*_iecInPort & _clkPinMask) != 0)
        {
            if (timeoutUs > 0 && timeUs > timeoutUs)
            {
                return false;
            }
            
            _delay_us(10);
            timeUs += 10;
        }
        
        return true;
    }
    
    void waitForNotClk()
    {
        while ((*_iecInPort & _clkPinMask) == 0);
    }
    
    bool clkTrue()
    {
        return ((*_iecInPort & _clkPinMask) == 0);
    }
    
    // wait for either the ATN or CLK line to be released
    void waitForNotAtnOrNotClk()
    {
        while (atnTrue() && clkTrue());
    }
    
    void setDataTrue()
    {
        // set data pin as output
        unsigned char reg = *_iecReg;
        reg |= _dataPinMask;
        *_iecReg = reg;
    }
    
    void setDataFalse()
    {
        unsigned char reg = *_iecReg;
        reg &= ~_dataPinMask;
        *_iecReg = reg;
    }
    
    unsigned char readDataByte()
    {
        unsigned char byte = 0;
        for (int i = 0; i < 8; i++)
        {
            byte >>= 1;
            // wait for clock
            waitForNotClk();
            
            // get bit
            if ((*_iecInPort & _dataPinMask) != 0)
            {
                byte |= 0x80;
            }
            
            waitForClk();
        }
        
        return byte;
    }
    
    unsigned char readByteWithHandshake(bool& lastByte)
    {
        unsigned char byte;
        lastByte = false;
        // wait for talker to be ready
        //waitForNotClk();
        
        // signal ready by releasing data line
        setDataFalse();
        
        // talker will pull clock to true in < 200us
        // if not, need to perform EOI (last byte)
        if (!waitForClk(200))
        {
            // timed out waiting for clock to be set true
            // this is an EOI signaled by the talker (last byte)
            // pull data line true for at least 60 ms
            setDataTrue();
            lastByte = true;
            _delay_us(60);
            setDataFalse();
            waitForClk();
        }
        
        byte = readDataByte();
        
        setDataTrue();
        
        return byte;
    }
    
private:
    volatile unsigned char* _iecOutPort;
    volatile unsigned char* _iecInPort;
    volatile unsigned char* _iecReg;
    
    unsigned char _atnPinMask;
    unsigned char _clkPinMask;
    unsigned char _dataPinMask;
};



void log(const char* string)
{
    spi_data.sendAndRecvPacket((unsigned char*)string, strlen(string));
}

void port_init(void)
{
}

//call this routine to initialize all peripherals
void init_devices(void)
{
    cli();  //all interrupts disabled
    port_init();
}

typedef enum
{
    WaitingForAtn = 0,
    ReadingAtn = 1,
    Listening = 2,
    Talking = 3,
    Unlisten = 4
} DeviceState;

typedef struct
{
    unsigned char state;
    unsigned char ss;
    bool isAtn;
    unsigned char size;
    unsigned char buffer[256];
    unsigned char atn_size;
    unsigned char atn_buffer[64];
} dataPacket;

int main(void)
{
    DDRD = 0xC0;
    PORTD = 0x00;
    dataPacket pkt;
    spi_data.spi_init();
    
    c64iec iec(&PORTC, &PINC, &DDRC, PC0, PC1, PC2);
    
    unsigned char bb;
    unsigned char temp;
    bool isLastByte;
    DeviceState state = WaitingForAtn;
    DeviceState nextState = Unlisten;
    pkt.size = 0;
    pkt.atn_size = 0;
    
    while (1)
    {
        if (state == WaitingForAtn)
        {
            PORTD = 0x00;
            iec.waitForAtn();
            iec.waitForClk();
            iec.setDataTrue();
            state = ReadingAtn;
            //state = Listening;
        }
        if (state == ReadingAtn)
        {
            iec.waitForNotAtnOrNotClk();
            if (iec.atnTrue())
            {
                if (pkt.size > 0 || pkt.atn_size > 0)
                {
                    spi_data.sendAndRecvPacket((unsigned char*)&pkt, sizeof(pkt));
                    pkt.size = 0;
                    pkt.atn_size = 0;
                }
                
                PORTD = 0x80;
                // read one atn byte
                temp = iec.readByteWithHandshake(isLastByte);
                PORTD = 0x00;
                
                pkt.atn_buffer[pkt.atn_size++] = temp;
                
                if (temp == 0x3F)
                {
                    nextState = Unlisten;
                }
                //else if ((temp & 0x20) != 0)
                else if (temp >= 0x20 && temp < 0x3F)
                {
                    // listen
                    nextState = Listening;
                }
                else if (temp == 0xE1)
                {
                    PORTD = 0x40;
                }
                /*
                else if (temp == 0x5F)
                {
                    // untalk
                }
                else if ((temp & 0x40) != 0)
                {
                    nextState = Talking;
                }
                */
            }
            else
            {
                // atn has deasserted
                // switch to next state
                state = nextState;
                nextState = Unlisten;
                
                /*
                if (state == Unlisten)
                {
                    spi_data.sendAndRecvPacket((unsigned char*)&pkt, sizeof(pkt));
                    pkt.size = 0;
                    pkt.atn_size = 0;
                    nextState = Unlisten;
                }
                */
            }
        }
        else if (state == Listening)
        {
            PORTD = 0x80;
            //iec.waitForNotClk();
            
            while (iec.clkTrue() && !iec.atnTrue());
            if (iec.atnTrue())
            {
                // atn was asserted, read atn bytes
                state = ReadingAtn;
                continue;
            }
            
            pkt.buffer[pkt.size++] = iec.readByteWithHandshake(isLastByte);
            
            if (isLastByte)
            {
                state = Unlisten;
            }
            
            
            /*
            PORTD = 0x80;
            iec.waitForNotClk();
            
            // if there is data in the packet, and we are about to receive an ATN command,
            // flush remaining data
            bool isAtn = iec.atnTrue();
            if (pkt.size > 0 && isAtn)
            {
                pkt.ss = state;
                spi_data.sendAndRecvPacket((unsigned char*)&pkt, sizeof(pkt));
                pkt.size = 0;
                pkt.atn_size = 0;
            }
            
            temp = iec.readByteWithHandshake(isLastByte);
            if (isAtn)
            {
                // this is an atn byte, not data
                pkt.atn_buffer[pkt.atn_size++] = temp;
            }
            else
            {
                // this is a data byte
                pkt.buffer[pkt.size++] = temp;
            }
            
            if (isLastByte || temp == UNLISTEN)
            {
                state = Unlisten;
            }
            
            // about to unlisten the bus, wait for atn to deassert
            if (temp == UNLISTEN)
            {
                iec.waitForNotAtn();
            }
            */
        }
        else if (state == Talking)
        {
            
        }
        else if (state == Unlisten)
        {
            _delay_us(60);
            iec.setDataFalse();
            state = WaitingForAtn;
        }
    }
    
    /*
     else if (state == Listening)
     {
     PORTD = 0x80;
     iec.waitForNotClk();
     
     // if there is data in the packet, and we are about to receive an ATN command,
     // flush remaining data
     bool isAtn = iec.atnTrue();
     if (pkt.size > 0 && isAtn)
     {
     pkt.ss = state;
     spi_data.sendAndRecvPacket((unsigned char*)&pkt, sizeof(pkt));
     pkt.size = 0;
     pkt.atn_size = 0;
     }
     
     temp = iec.readByteWithHandshake(isLastByte);
     if (isAtn)
     {
     // this is an atn byte, not data
     pkt.atn_buffer[pkt.atn_size++] = temp;
     }
     else
     {
     // this is a data byte
     pkt.buffer[pkt.size++] = temp;
     }
     
     if (isLastByte || temp == UNLISTEN)
     {
     state = Unlisten;
     }
     
     // about to unlisten the bus, wait for atn to deassert
     if (temp == UNLISTEN)
     {
     iec.waitForNotAtn();
     }
     }
     */
}
