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
        //waitForClk();
        if (!waitForClk(200))
        {
            // timed out waiting for clock to be set true
            // this is an EOI signaled by the talker (last byte)
            // pull data line true for at least 60 ms
            setDataTrue();
            lastByte = true;
            _delay_us(60);
            setDataFalse();
        }
        
        byte = readDataByte();
        
        // signal byte received
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
    WaitingForAtn,
    ReadingAtn,
    Listening,
    Talking,
    Unlisten
} DeviceState;

int main(void)
{
    unsigned char buffer[512];
    spi_data.spi_init();
    
    c64iec iec(&PORTC, &PINC, &DDRC, PC0, PC1, PC2);
    //log("initialized.");
    
    unsigned char bb;
    unsigned char temp;
    int byteCount;
    bool isLastByte;
    DeviceState state = WaitingForAtn;
    
    while (1)
    {
        if (state == WaitingForAtn)
        {
            byteCount = 0;
            iec.waitForAtn();
            iec.waitForClk();
            iec.setDataTrue();
            state = ReadingAtn;
        }
        else if (state == ReadingAtn)
        {
            // wait for either another byte to be sent or atn to be released
            //iec.waitForNotClk();
            iec.waitForNotAtnOrNotClk();
            
            // check if ATN is still asserted
            if (!iec.atnTrue())
            {
                temp = buffer[byteCount-1];
                spi_data.sendAndRecvPacket(buffer, byteCount);
                
                // determine next state
                if (temp == 0x3F)
                {
                    state = Unlisten;
                }
                else if (temp == 0xF1)
                {
                    byteCount = 0;
                    state = Listening;
                }
                else if (temp == 0x61)
                {
                    byteCount = 0;
                    state = Listening;
                }
                else
                {
                    state = Unlisten;
                }
            }
            else
            {
                buffer[byteCount++] = iec.readByteWithHandshake(isLastByte);
            }
        }
        else if (state == Listening)
        {
            iec.waitForNotClk();
            buffer[byteCount++] = iec.readByteWithHandshake(isLastByte);
            if (isLastByte)
            {
                // release data line
                spi_data.sendAndRecvPacket(buffer, byteCount);
                state = Unlisten;
            }
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
    
    
    while (1)
    {
        iec.waitForAtn();
        // got ATN
        
        // wait for talker to lower CLK
        iec.waitForClk();
        
        // lower data to signal ready
        iec.setDataTrue();
        
        // wait for talker to be ready
        iec.waitForNotClk();
        
        // signal ready by releasing data line
        iec.setDataFalse();
        
        // talker will pull clock to true in < 200us
        // if not, need to perform EOI (last byte)
        iec.waitForClk();
         
        bb = iec.readDataByte();
        
        // signal byte received
        iec.setDataTrue();
        
        if (bb == 0x3F)
        {
            // unlisten
            spi_data.sendAndRecvPacket(&bb, 1);
            iec.setDataFalse();
            continue;
        }
        else
        {
            spi_data.sendAndRecvPacket(&bb, 1);
        }
        
        bool done = false;
        bool saving = false;
        int bytes = 0;
        while (!done)
        {
            // wait for talker to signal that a byte is coming
            iec.waitForNotClk();
            
            // respond by releasing data line
            iec.setDataFalse();
            
            if (!iec.waitForClk(200))
            {
                // timed out waiting for clock to be set true
                // this is an EOI signaled by the talker (last byte)
                // pull data line true for at least 60 ms
                iec.setDataTrue();
                done = true;
                _delay_us(60);
                iec.setDataFalse();
            }
            
            //buffer[bytes++] = iec.readDataByte();
            bb = iec.readDataByte();
            
            // signal byte received
            iec.setDataTrue();
            temp = bb;
            spi_data.sendAndRecvPacket(&temp, 1);
            
            if (saving)
            {
                buffer[bytes++] = bb;
            }
            
            if (bb == 0x61)
            {
                saving = true;
            }
            else if (bb == 0x60)
            {
                
            }
        }
        
        // delay and release data line, transmission is finished
        if (bytes > 0)
        {
            spi_data.sendAndRecvPacket(buffer, bytes);
        }
        _delay_us(60);
        iec.setDataFalse();
    }
    
}
