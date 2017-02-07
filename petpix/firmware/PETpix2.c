/*
 * PETpix2.c
 *
 * Created: 2/2/2013 9:14:06 PM
 *  Author: Michael Hill
 */ 

//#include "lz.h"
#include "UART_routines.h"
#include <avr/io.h>
#include <util/delay.h>

//#define ACTIVE_LOW 1
//#define DEBUG 1

//#define USE_COMPRESSED_BUFFER 1

int main(void)
{
    unsigned char txReady;
    unsigned char txNotReady;
    unsigned char transmitColorBytes = 0;
    
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
    
    uart0_init(2); // 333kbaud
    //uart0_init(51); // 19.2kbaud
    
    // delay to get falling edge
    _delay_loop_2(1000);
    
    unsigned char byte;
    unsigned int numBytes;
    
#ifdef USE_COMPRESSED_BUFFER
    unsigned char compressedBuffer[1100];
#endif
    
    unsigned char buffer[2000];
    unsigned char page = 0;
	unsigned char color = 0;
    int i;
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
        
#ifdef USE_COMPRESSED_BUFFER
        buffer[0] = receiveByte();
        buffer[1] = receiveByte();
        
        numBytes = buffer[0];
        numBytes = numBytes << 8;
        numBytes += buffer[1];
        
        //transmitHex(INT, numBytes);

        for (i = 0; i < numBytes; i++)
        {
            compressedBuffer[i] = receiveByte();
        }
        
        transmitHex(INT, numBytes);
        //Huffman_Uncompress(compressedBuffer, buffer, numBytes, 1000);
        //RLE_Uncompress(compressedBuffer, buffer, numBytes);
        LZ_Uncompress(compressedBuffer, buffer, numBytes);
#else

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
                //byte = receiveByte();
            }
        }
        
        // TEST TEST
        //_delay_loop_2(2000);
#endif
        
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
}