/*
 * upserial.c
 * userport serial adapter
 *
 * Created: 3/15/2016
 *  Author: Michael Hill
 */ 

#include "UART_routines.h"
#include <avr/io.h>
#include <util/delay.h>

//#define ACTIVE_LOW 1
//#define DEBUG 1

int main(void)
{
    unsigned char txReady;
    unsigned char txNotReady;
    
#ifdef ACTIVE_LOW
    txReady = 0x00;
    txNotReady = 0x01;
#else
    txReady = 0x01;
    txNotReady = 0x00;
#endif
    
    // set CA1 output
    DDRA = 0x01;
    // set low
    PORTA = txReady;
    
    DDRD = 0x40;
    PORTD = 0x40;
    
    DDRC = 0xFF;
    
    uart0_init(51); // 19.2kbaud
    
    // delay to get falling edge
    _delay_loop_2(1000);
    
    unsigned char byte;
    unsigned int numBytes;
    
    unsigned char buffer[2000];
    unsigned char page = 0;
	unsigned char color = 0;
    int i;
    
    transmitString("hello.\n");
    
    PORTC = 0x00;
    
    while (1)
    {
        byte = receiveByte();
        transmitByte(byte);
        transmitByte('\n');
        PORTC = byte;
    }
    
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