/*
 * upserial.c
 * userport serial adapter
 *
 * Created: 3/15/2016
 *  Author: Michael Hill
 */ 

#include "utils.h"
#include "UART_routines.h"
#include <avr/io.h>
#include <util/delay.h>

//#define ACTIVE_LOW 1
//#define DEBUG 1

#define TX_READY        0x00
#define TX_NOT_READY    0x01

typedef enum
{
    IDLE,
    BYTE_READY,
    WAIT_FOR_REMOTE_READY
} state;

void output_byte(unsigned char byte)
{
    PORTC = byte;
}

void signal_byte_ready()
{
    PORTA = TX_READY;
}

void signal_byte_not_ready()
{
    PORTA = TX_NOT_READY;
}

bool read_CB2()
{
    unsigned char cv;
    cv = PINA;
    cv = cv & 0x02;
    if (cv == 0x00)
    {
        return false;
    }
    
    return true;
}

bool remote_has_read_byte()
{
    bool cb2 = read_CB2();
    if (!cb2)
    {
        return true;
    }
    
    return false;
}

bool remote_ready()
{
    bool cb2 = read_CB2();
    if (cb2)
    {
        return true;
    }
    
    return false;
}

int main(void)
{
    // set CA1 output
    DDRA = 0x01;
    DDRD = 0x40;
    PORTD = 0x40;
    
    // all data lines output
    DDRC = 0xFF;
    uart0_init(51); // 19.2kbaud
    signal_byte_not_ready();
    
    // delay to get falling edge
    _delay_loop_2(1000);
    PORTC = 0x00;
    
    circularBuffer byteBuffer;
    circularBuffer_init(&byteBuffer);
    
    circularBuffer outputBuffer;
    circularBuffer_init(&outputBuffer);
    
    state st = IDLE;
    
    // main loop
    while (1)
    {
        // check for new character from uart
        if (byteAvailable())
        {
            // read the byte
            unsigned char byte = receiveByte();
            call1(byteBuffer, push, byte);
        }
        
        // send character if possible
        if (call(outputBuffer, getSize) > 0)
        {
            if (transmitReady())
            {
                unsigned char byte = call(outputBuffer, pop);
                transmitByte(byte);
            }
        }
        
        if (st == IDLE)
        {
            // check for available bytes in buffer
            if (call(byteBuffer, getSize) > 0)
            {
                unsigned char byte = call(byteBuffer, pop);
                // put byte on data lines
                output_byte(byte);
                
                // echo byte back
                call1(outputBuffer, push, byte);
                
                // signal byte ready
                signal_byte_ready();
                
                // change state
                st = BYTE_READY;
                
                //call1(outputBuffer, push, 'a');
            }
        }
        else if (st == BYTE_READY)
        {
            // check for remote to read a byte
            if (remote_has_read_byte())
            {
                // unset byte ready signal
                signal_byte_not_ready();
                
                st = WAIT_FOR_REMOTE_READY;
                
                //call1(outputBuffer, push, 'b');
            }
        }
        else if (st == WAIT_FOR_REMOTE_READY)
        {
            // check if remote is ready
            if (remote_ready())
            {
                // return to idle state
                st = IDLE;
                
                //call1(outputBuffer, push, 'c');
            }
        }
    }
    
    
    /*
    const char *str = "What's up dude? Fart";
    
    // test
    for (unsigned char i = 0; i < 10; i++)
    {
        (byteBuffer.push)(&byteBuffer, str[i]);
    }
    
    for (int i = 0; i < 10; i++)
    {
        int s;
        unsigned char c;
        
        s = (byteBuffer.getSize)(&byteBuffer);
        transmitHex(INT, s);
        if (s > 0)
        {
            c = (byteBuffer.pop)(&byteBuffer);
            transmitString((unsigned char *)"popped: ");
            transmitHex(CHAR, c);
            transmitString((unsigned char *)" ");
            transmitByte(c);
            transmitString((unsigned char *)"\r\n");
        }
    }
    */
    
    
    
    
    
    /*
    
    while(1)
    {
        // set CA1 line low
        PORTA = txNotReady;
        byte = 0x00;
        
#ifdef DEBUG
        transmitHex(CHAR, txReady);
        transmitHex(CHAR, txNotReady);
        transmitString("\r\n");
#endif
        
        
        transmitByte(':');
    
        // skip over buffer bytes
        while (byte != 0xff)
        {
            byte = receiveByte();
        }
        
        while (byte != 0x00)
        {
            byte = receiveByte();
        }

        if (transmitColorBytes == 1)
        {
            numBytes = 2000;
            for (i = 0; i < numBytes; i += 2)
            {
                buffer[i] = receiveByte();
                buffer[i+1] = receiveByte();
            }
        }
        else
        {
            numBytes = 1000;
            for (i = 0; i < numBytes; i++)
            {
                buffer[i] = receiveByte();
                byte = receiveByte();
            }
        }
        
        int j = numBytes;
        unsigned char cv, currentVal;
        
        // done getting data
        DDRC = 0xFF;
        PORTA = txReady;
        
#ifdef DEBUG
        transmitString("waiting for CB2");
#endif
    
        
        // now wait for CB2 to go low
        cv = PINA;
        cv = cv & 0x02;
        while (cv != 0x00)
        {
            cv = PINA;
            cv = cv & 0x02;
        }
        
        //transmitString("got CB2 low");
        
        // indicate not ready, test
        PORTA = txNotReady;
        
        
        // CB2 is now low
        currentVal = cv;
        for (i = 0; i < j; i++)
        {
            // output the current byte
            PORTC = buffer[i];
            
            // wait for output to toggle
            cv = PINA;
            cv = cv & 0x02;
            while (cv == 0x00)
            {
                cv = PINA;
                cv = cv & 0x02;
            }
            
            i++;
            PORTC = buffer[i];
            
            //transmitHex(INT, i);
            
            if (i < j-1)
            {
                cv = PINA;
                cv = cv & 0x02;
                while (cv != 0x00)
                {
                    cv = PINA;
                    cv = cv & 0x02;
                }
            }
            
            // the value has toggled, send next byte
        }
    }
    */
}