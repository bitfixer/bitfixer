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

void init()
{
    // set CA1 output
    DDRA = 0x01;
    DDRD = 0x40;
    PORTD = 0x40;
    
    // all data lines output
    DDRC = 0xFF;
    //uart0_init(51); // 19.2kbaud
    uart0_init(25); // 38.4kbaud
    //uart0_init(16); // 57.8kbaud
    //uart0_init(12); // 76.8kbaud
    //uart0_init(8); // 115.2kbaud
    //uart0_init(3);
}

int main(void)
{
    init();
    signal_byte_not_ready();
    
    // delay to get falling edge
    _delay_loop_2(1000);
    output_byte(0x00);
    
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
            }
        }
        else if (st == WAIT_FOR_REMOTE_READY)
        {
            // check if remote is ready
            if (remote_ready())
            {
                // return to idle state
                st = IDLE;
            }
        }
    }
}