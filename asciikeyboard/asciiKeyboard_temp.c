/*
 * PETpix2.c
 *
 * Created: 2/2/2013 9:14:06 PM
 *  Author: Michael Hill
 */

#include "UART_routines.h"
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
    // set CA1 output
    //DDRA = 0x01;
    uart0_init(25);

    unsigned char ch;
    while(1)
    {
        transmitByte(':');
        ch = receiveByte();
        transmitByte(ch);
        transmitByte(ch);
    }
}
