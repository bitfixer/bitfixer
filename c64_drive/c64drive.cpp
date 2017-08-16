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
#include "c64drive.h"
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

#define LISTEN   0x20
#define UNLISTEN 0x3F
#define TALK     0x40
#define UNTALK   0x5F

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
    
    // pull down the clock line
    void setClockTrue()
    {
        unsigned char reg = *_iecReg;
        reg |= _clkPinMask;
        *_iecReg = reg;
    }
    
    void setClockFalse()
    {
        unsigned char reg = *_iecReg;
        reg &= ~_clkPinMask;
        *_iecReg = reg;
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
    
    void waitForData()
    {
        while ((*_iecInPort & _dataPinMask) != 0);
    }
    
    void waitForNotData()
    {
        while ((*_iecInPort & _dataPinMask) == 0);
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
    
    void writeByte(unsigned char byte, bool lastByte)
    {
        if (lastByte)
        {
            // last byte, need to indicate eoi
            // wait at least 200 us
            _delay_us(200);
            
            // wait for listener response
            waitForData();
            // wait for pulse to end
            waitForNotData();
        }
        
        _delay_us(30);
        setClockTrue();
        
        // output 8 bits
        for (int i = 0; i < 8; i++)
        {
            // output bit
            if ((byte & 0x01) == 0)
            {
                // output a zero
                setDataTrue();
            }
            else
            {
                setDataFalse();
            }
            
            // shift byte
            byte >>= 1;
            
            _delay_us(70);
            setClockFalse();
            
            _delay_us(70);
            setClockTrue();
        }
        // release data line
        setDataFalse();
        _delay_us(20);
        
        // wait for listener to accept
        waitForData();
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
    TalkTurnaround = 3,
    Talking = 4,
    Unlisten = 5
} DeviceState;

int main(void)
{
    DDRD = 0xE0;
    PORTD = 0x00;
    dataPacket pkt;
    spi_data.spi_init();
    
    c64iec iec(&PORTC, &PINC, &DDRC, PC0, PC1, PC2);
    
    unsigned char bb;
    unsigned char temp;
    bool isLastByte;
    int pgmindex = 0;
    DeviceState state = WaitingForAtn;
    DeviceState nextState = Unlisten;
    pkt.data_size = 0;
    pkt.atn_size = 0;
    
    while (1)
    {
        if (state == WaitingForAtn)
        {
            PORTD = 0x20;
            iec.waitForAtn();
            PORTD = 0x00;
            
            // release clock
            iec.setClockFalse();
            
            iec.waitForClk();
            iec.setDataTrue();
            state = ReadingAtn;
        }
        if (state == ReadingAtn)
        {
            PORTD = 0x20;
            iec.waitForNotAtnOrNotClk();
            if (iec.atnTrue())
            {
                if (pkt.data_size > 0 || pkt.atn_size > 0)
                {
                    spi_data.sendAndRecvPacket((unsigned char*)&pkt, sizeof(pkt));
                    pkt.data_size = 0;
                    pkt.atn_size = 0;
                }
                
                // read one atn byte
                temp = iec.readByteWithHandshake(isLastByte);
                
                pkt.atn_buffer[pkt.atn_size++] = temp;
                
                if (temp >= LISTEN && temp < UNLISTEN)
                {
                    // listen
                    nextState = Listening;
                }
                else if (temp == UNLISTEN)
                {
                    nextState = Unlisten;
                }
                else if (temp >= TALK && temp < UNTALK)
                {
                    nextState = TalkTurnaround;
                }
                else if (temp == UNTALK)
                {
                    nextState = Unlisten;
                }
            }
            else
            {
                // atn has deasserted
                // switch to next state
                state = nextState;
                nextState = Unlisten;
            }
            PORTD = 0x00;
        }
        else if (state == Listening)
        {
            while (iec.clkTrue() && !iec.atnTrue());
            if (iec.atnTrue())
            {
                // atn was asserted, read atn bytes
                state = ReadingAtn;
                continue;
            }
            
            // if buffer is full, send now
            if (pkt.data_size == 128)
            {
                spi_data.sendAndRecvPacket((unsigned char*)&pkt, sizeof(pkt));
                pkt.data_size = 0;
                pkt.atn_size = 0;
            }
            
            pkt.data_buffer[pkt.data_size++] = iec.readByteWithHandshake(isLastByte);
            
            if (isLastByte)
            {
                state = Unlisten;
            }
        }
        else if (state == TalkTurnaround)
        {
            // talker will hold data line down
            // wait for talker to release clock line
            iec.waitForNotClk();
            
            // release data line
            iec.setDataFalse();
            
            // pull the clock line
            iec.setClockTrue();
            _delay_us(80);
            
            state = Talking;
        }
        else if (state == Talking)
        {
            // if we do not have any bytes to send, ask the server
            if (pkt.atn_size > 0 || (pkt.data_size > 0 && pgmindex == pkt.data_size))
            {
                spi_data.sendAndRecvPacket((unsigned char*)&pkt, sizeof(pkt));
                pgmindex = 0;
                pkt.atn_size = 0;
            }
            
            bool lastByte = ((pgmindex+1 == pkt.data_size) && pkt.is_last_data_buffer);
            
            // indicate ready to send
            iec.setClockFalse();
            
            // wait for listener to indicate ready
            iec.waitForNotData();
            
            // if this is the final data buffer (indicated by server)
            // and this is the last byte in this data buffer, it's the last byte overall
            iec.writeByte(pkt.data_buffer[pgmindex++], lastByte);
            
            if (lastByte)
            {
                PORTD = 0x80;
                pkt.data_size = 0;
                state = Unlisten;
            }
            else
            {
                // wait for gap between bytes
                _delay_us(100);
            }
        }
        else if (state == Unlisten)
        {
            _delay_us(60);
            iec.setDataFalse();
            state = WaitingForAtn;
            PORTD = 0x00;
        }
    }
}
