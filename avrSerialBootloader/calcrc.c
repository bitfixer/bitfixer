// This file has been prepared for Doxygen automatic documentation generation.
/*! \file ********************************************************************
*
* Atmel Corporation
*
* File              : calcrc.c
* Compiler          : IAR EWAAVR 4.11a
* Revision          : $Revision: 2.0 $
* Date              : $Date: 23. august 2005 15:57:40 $
* Updated by        : $Author: jtyssoe $
*
* Support mail      : avr@atmel.com
*
* Supported devices : ATmega48/88/168
*
* AppNote           : AVR350: XmodemCRC Receive Utility for AVR
*                     Migrated to mega48 and corrected several bugs in original AT90S8515 code
*
* Description       : CRC calculation
*
****************************************************************************/

//#include "xmodem.h"

int calcrc(unsigned char *ptr, int count)
{
    int crc;
    char i;

    crc = 0;
    while (--count >= 0)
    {
        crc = crc ^ (int) *ptr++ << 8;
        i = 8;
        do
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
        } while(--i);
    }
    return (crc);
}
