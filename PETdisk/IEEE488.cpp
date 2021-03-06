/*
    IEEE488.c
    IEEE Routines in the PETdisk storage device
    Copyright (C) 2011 Michael Hill

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


#include "IEEE488.h"
#include "PETdisk.h"
#include "DataSource.h"
#include "Utils.h"

extern "C" {
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <inttypes.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include "UART_routines.h"
}

const unsigned char _blocksFree[] PROGMEM = "BLOCKS FREE.";

void wait_for_dav_high()
{
    unsigned char rdchar;
    rdchar = PINC;
    rdchar = rdchar & DAV;
    while (rdchar == 0x00)
    {
        rdchar = PINC;
        rdchar = rdchar & DAV;
    }
}

void wait_for_dav_low()
{
    unsigned char rdchar;
    rdchar = PINC;
    rdchar = rdchar & DAV;
    while (rdchar != 0x00)
    {
        rdchar = PINC;
        rdchar = rdchar & DAV;
    }
}

void wait_for_atn_high()
{
    unsigned char rdchar;
    rdchar = PINC;
    rdchar = rdchar & ATN;
    while (rdchar == 0x00)
    {
        rdchar = PINC;
        rdchar = rdchar & ATN;
    }
}

void wait_for_atn_low()
{
    unsigned char rdchar;
    rdchar = PINC;
    rdchar = rdchar & ATN;
    while (rdchar != 0x00)
    {
        rdchar = PINC;
        rdchar = rdchar & ATN;
    }
}

unsigned char wait_for_nrfd_high_or_atn_low()
{
    unsigned char rdchar;
    while (1)
    {
        rdchar = PINC;
        if ((rdchar & ATN) == 0x00)
        {
            return ATN;
        }
        else if ((rdchar & NRFD) != 0x00)
        {
            return NRFD;
        }
    }
}

void wait_for_nrfd_high()
{
    unsigned char rdchar;
    rdchar = PINC;
    rdchar = rdchar & NRFD;
    while (rdchar == 0x00)
    {
        rdchar = PINC;
        rdchar = rdchar & NRFD;
    }
}

void wait_for_nrfd_low()
{
    unsigned char rdchar;
    rdchar = PINC;
    rdchar = rdchar & NRFD;
    while (rdchar != 0x00)
    {
        rdchar = PINC;
        rdchar = rdchar & NRFD;
    }
}

void wait_for_ndac_low()
{
    unsigned char rdchar;
    rdchar = PINC;
    rdchar = rdchar & NDAC;
    while (rdchar != 0x00)
    {
        rdchar = PINC;
        rdchar = rdchar & NDAC;
    }
}

unsigned char wait_for_ndac_low_or_atn_low()
{
    unsigned char rdchar;
    while (1)
    {
        rdchar = PINC;
        if ((rdchar & ATN) == 0x00)
        {
            return ATN;
        }
        else if ((rdchar & NDAC) == 0x00)
        {
            return NDAC;
        }
    }
}

unsigned char wait_for_ndac_high_or_atn_low()
{
    unsigned char rdchar;
    while (1)
    {
        rdchar = PINC;
        if ((rdchar & ATN) == 0x00)
        {
            return ATN;
        }
        else if ((rdchar & NDAC) != 0x00)
        {
            return NDAC;
        }
    }
}

void wait_for_ndac_high()
{
    unsigned char rdchar;
    rdchar = PINC;
    rdchar = rdchar & NDAC;
    while (rdchar == 0x00)
    {
        rdchar = PINC;
        rdchar = rdchar & NDAC;
    }
}

void recv_byte_IEEE(unsigned char *byte)
{
    unsigned char hi, lo;
    hi = DATA_HI_IN;
    lo = DATA_LO_IN;

    hi = hi & ~DATALO;
    lo = lo & DATALO;

    *byte = ~(hi | lo);

}

void send_byte(unsigned char byte, int last)
{
    unsigned char temp,temp2;
    unsigned char lo,hi;
    // invert the byte
    temp = ~byte;
    lo = temp & DATALO;
    hi = temp & ~DATALO;


    // output the byte
    //PORTA = temp;
    DATA_PORT = hi;
    temp = PORTB;
    temp = temp & ~DATALO;
    temp = temp | lo;
    PORTB = temp;

    // wait for NRFD high
    wait_for_nrfd_high();

    // lower DAV and EOI

    if (last == 0)
    {
        temp = DAV;
        PORTC = ~temp;
    }
    else
    {
        temp = DAV;
        temp2 = EOI;
        PORTC = (~temp) & (~temp2);
    }

    // wait for NDAC high
    wait_for_ndac_high();

    // raise DAV
    temp = DAV | EOI;
    // output to bus
    PORTC = temp;

}

void unlisten()
{
    // all bus lines input
    IEEE_CTL = 0x00;
    // set pullups
    IEEE_PORT = 0xff;
}

unsigned char wait_for_device_address(unsigned char my_address)
{
    unsigned char primary_address, dir;
    primary_address = 0;
    dir = 0;
    //transmitString("waiting..");
    while (primary_address != my_address)
    {
        // wait for atn signal
        wait_for_atn_low();

        // lower NDAC to respond
        IEEE_PORT = NOT_NDAC;
        // set output lines
        IEEE_CTL = NDAC | NRFD;

        // wait for primary address
        wait_for_dav_low();

        // lower NRFD
        IEEE_PORT = NOT_NDAC & NOT_NRFD;

        // read data
        recv_byte_IEEE(&primary_address);
        dir = primary_address & 0xf0;
        primary_address = primary_address & 0x0f;
        //transmitHex(CHAR, primary_address);

        if (primary_address == my_address && (dir == TALK || dir == LISTEN))
        {
            // raise NDAC
            IEEE_PORT = NOT_NRFD;
            // wait for data to finish
            wait_for_dav_high();

            // release control and handle transaction
            IEEE_PORT = NOT_NDAC;
        }
        else
        {
            // unlisten the bus
            unlisten();
            // wait for atn to release
            wait_for_atn_high();
        }
    }
    return dir;
}

unsigned char sendIEEEByteCheckForATN(unsigned char byte)
{
    unsigned char temp,temp2;
    unsigned char lo,hi;
    unsigned char result = 0;
    // invert the byte
    temp = ~byte;
    lo = temp & DATALO;
    hi = temp & ~DATALO;

    // output the byte
    DATA_PORT = hi;
    temp = PORTB;
    temp = temp & ~DATALO;
    temp = temp | lo;
    PORTB = temp;

    result = wait_for_ndac_low_or_atn_low();
    if (result == ATN)
    {
        return result;
    }

    // wait for NRFD high
    result = wait_for_nrfd_high_or_atn_low();
    if (result == ATN)
    {
        return result;
    }

    // lower DAV
    temp = DAV;
    PORTC = ~temp;
}

void sendIEEEBytes(unsigned char *entry, int size, unsigned char isLast)
{
    int i;
    int last = size;

    if (isLast)
    {
        last--;
    }

    for (i = 0; i < last; i++)
    {
        send_byte(entry[i], 0);
    }

    if (isLast)
    {
        send_byte(entry[i], 1);
    }
}

void ListFilesIEEE(void* dataSource, unsigned char* buffer)
{
    unsigned int dir_start; // address of directory start
    unsigned char entry[32];
    unsigned int file = 0;
    DataSource* ds = (DataSource*)dataSource;
    DirectoryEntry* dirent = 0;

    dir_start = 0x041f;

    // open remote directory
    ds->openDirectory();

    do
    {
        // get next directory entry
        dirent = ds->getNextDirectoryEntry(buffer);
        // clear entry
        memset(entry, ' ', 32);

        if (dirent == 0) // this is the end of the directory
        {
            // write ending bytes
            int startline = 0;
            dir_start += 0x001e;

            entry[startline] = (unsigned char)(dir_start & 0x00ff);
            entry[startline+1] = (unsigned char)((dir_start & 0xff00) >> 8);
            entry[startline+2] = 0xff;
            entry[startline+3] = 0xff;

            pgm_memcpy(&entry[startline+4], _blocksFree, 12);

            entry[startline+29] = 0x00;
            entry[startline+30] = 0x00;
            entry[startline+31] = 0x00;

            sendIEEEBytes(entry, 32, 1);
            return;
        }
        else
        {
            dir_start += 0x0020;

            int startline = 0;
            int fname_length = dirent->name_length;

            entry[startline] = (unsigned char)(dir_start & 0x00ff);
            entry[startline+1] = (unsigned char)((dir_start & 0xff00) >> 8);
            entry[startline+2] = file+1;
            entry[startline+3] = 0x00;

            entry[startline+6] = '"';
            strncpy((char*)&entry[startline+7], (const char*)dirent->name, fname_length);
            entry[startline+7+fname_length] = '"';

            strncpy((char*)&entry[startline+25], (const char*)dirent->ext, 3);
            entry[startline+31] = 0x00;
            file++;

            sendIEEEBytes(entry, 32, 0);
        }
    }
    while (dirent != 0);
}

void writeFileFromIEEE(void* dataSource, unsigned char* buffer)
{
    unsigned int numBytes;
    unsigned char rdchar;
    unsigned char rdbus;
    int bytes_in_buffer = 512;
    DataSource* ds = (DataSource*)dataSource;

    numBytes = 0;
    do
    {
        wait_for_dav_low();
        PORTC = NOT_NDAC & NOT_NRFD;
        // read byte
        recv_byte_IEEE(&rdchar);
        rdbus = PINC;

        // read byte into buffer
        buffer[numBytes++] = rdchar;

        if (numBytes >= bytes_in_buffer)
        {
            // write buffer to datasource
            ds->writeBufferToFile(buffer, numBytes);
            numBytes = 0;
        }

        // raise NDAC
        PORTC = NOT_NRFD;
        wait_for_dav_high();
        PORTC = NOT_NDAC;
	}
    while((rdbus & EOI) != 0x00);

    if (numBytes > 0)
    {
        // write last block of file
        ds->writeBufferToFile(buffer, numBytes);
    }

    ds->closeFile();
}
