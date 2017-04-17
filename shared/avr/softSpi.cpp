//
//  softSpi.cpp
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

#include "softSpi.h"
#include <util/delay.h>

#define CSMASK  0x04
#define SCKMASK 0x20
#define MOSIMASK 0x10
#define MISOMASK 0x08
#define NOTMISOMASK 0xF7

void softSpi::init()
{
    if (_driveChipSelect)
    {
        // if driving the chip select line, enable it
        DDRB = CSMASK | MISOMASK;
        PORTB = CSMASK;
    }
    else
    {
        DDRB = MISOMASK;
        PORTB = 0x00;
    }
}

void softSpi::setMiso(unsigned char bit)
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

int softSpi::transfer(unsigned char* buffer, int size)
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
        // lower chip select line
        PORTB = 0x00;
    }
    else // read chip select from master
    {
        while (c != 0)
        {
            c = PINB & CSMASK;
        }
    }

    // assembly version
    asm volatile(
        "transferbyte:\n\t"
    // prepare input and output bytes
    // zero out input byte
        "ldi r20, 0x00" "\n\t"
    // load input value from buffer
        "ld r17, X" "\n\t"

    // prepare miso values
        "ldi r22, 0x00" "\n\t"
        "ldi r23, %2" "\n\t"

    // prepare counter
        "ldi r21, 8" "\n\t"
        "transferbit:\n\t"

    // shift left to check highest bit
        "lsl r17" "\n\t"
    // check C flag to present 0 or one on miso line
        "brcs sendmisohigh" "\n\t"
        "sendmisolow:" "\n\t"
            //"in r18, %1" "\n\t"
            //"andi r18, %3" "\n\t"
            "out %1, r22" "\n\t"
            "jmp waitclock" "\n\t"
            //"jmp presentmiso" "\n\t"
        "sendmisohigh:\n\t"
            "out %1, r23" "\n\t"
            //"in r18, %1" "\n\t"
            //"ori r18, %2" "\n\t"
        // present bit on miso line
        //"presentmiso:\n\t"
            //"out %1, r18" "\n\t"

    // wait for clock to go high
        "waitclock:\n\t"
            // read port
            "in r18, %4" "\n\t"
            // copy register
            "mov r19, r18" "\n\t"
            // check for clock
            "andi r18, %5" "\n\t"
            // clock high, go to reading input
            "brne readinputbit" "\n\t"
            // clock still low, check for cs
            "andi r19, %6" "\n\t"
            // cs high, done
            "brne done" "\n\t"
            "jmp waitclock" "\n\t"
        "readinputbit:\n\t"
            // rotate input byte
            "lsl r20" "\n\t"
            // get byte from port
            "in r18, %4" "\n\t"
            // check for mosi bit
            "andi r18, %7" "\n\t"
            // if mosi bit zero, skip
            "breq waitclocklow" "\n\t"
        "readbithigh:\n\t"
            "ori r20, 0x01" "\n\t"

/*
            // test
            "lsl r17" "\n\t"
            "brcs sndhigh" "\n\t"
            "mov r19, r22" "\n\t"
            "jmp waitclocklow" "\n\t"
            "sndhigh:" "\n\t"
            "mov r19, r23" "\n\t"
            */

        "waitclocklow:\n\t"
            // read port
            "in r18, %4" "\n\t"
            // check for clock
            "andi r18, %5" "\n\t"
            // if clock not zero, check again
            "brne waitclocklow" "\n\t"

            // output value
            //"out %1, r19" "\n\t"

        // decrement counter and check if done
        "dec r21" "\n\t"
        "brne transferbit" "\n\t"
        //"brne waitclock" "\n\t"

        // done with byte, copy to buffer
        "st X+, r20" "\n\t"
        "jmp transferbyte" "\n\t"
    // done
        "done:\n\t"
        :
        : "x" (buffer)
        , "I" (_SFR_IO_ADDR(PORTB))
        , "M" (MISOMASK)
        , "M" (NOTMISOMASK)
        , "I" (_SFR_IO_ADDR(PINB))
        , "M" (SCKMASK)
        , "M" (CSMASK)
        , "M" (MOSIMASK)
    );


/*
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
*/

    return bytesReceived;
}
