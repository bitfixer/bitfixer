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

extern "C" {
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <inttypes.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include "UART_routines.h"
//#include "FAT32.h"
//#include "sd_routines.h"
}

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
    //wait_for_nrfd_high();
    result = wait_for_nrfd_high_or_atn_low();
    if (result == ATN)
    {
        return result;
    }

    // lower DAV
    temp = DAV;
    PORTC = ~temp;

    /*
    // wait for NDAC high
    result = wait_for_ndac_high_or_atn_low();

    // raise DAV
    temp = DAV | EOI;
    // output to bus
    PORTC = temp;

    return result;
    */
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

void ListFilesIEEE(unsigned long firstCluster, void* dataSource, unsigned char* buffer)
{
    unsigned int dir_start; // address of directory start
    unsigned char entry[32];
    unsigned int file = 0;
    DataSource* ds = (DataSource*)dataSource;
    DirectoryEntry* dirent = 0;

    dir_start = 0x041f;

    // open remote directory
    ds->openDirectory();

    int tt = 2;
    do
    {
        // get next directory entry
        dirent = ds->getNextDirectoryEntry(buffer);
        if (dirent == 0) // this is the end of the directory
        {
            // write ending bytes
            int startline = 0;
            dir_start += 0x001e;

            entry[startline] = (unsigned char)(dir_start & 0x00ff);
            entry[startline+1] = (unsigned char)((dir_start & 0xff00) >> 8);
            entry[startline+2] = 0xff;
            entry[startline+3] = 0xff;
            sprintf((char *)&entry[startline+4], (char *)"BLOCKS FREE.             ");
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
            //transmitString(dirent->name);
            transmitHex(INT, fname_length);

            entry[startline] = (unsigned char)(dir_start & 0x00ff);
            entry[startline+1] = (unsigned char)((dir_start & 0xff00) >> 8);
            entry[startline+2] = file+1;
            entry[startline+3] = 0x00;
            entry[startline+4] = 0x20;
            entry[startline+5] = 0x20;
            entry[startline+6] = 0x22;

            for (int f = 0; f < 17; f++)
            {
                entry[startline+7+f] = ' ';
                //entry[startline+7+f] = dirent->name[f];
            }

            for (int f = 0; f < fname_length; f++)
            {
                entry[startline+7+f] = dirent->name[f];
            }
            entry[startline+7+fname_length] = 0x22;

            entry[startline+25] = dirent->ext[0];
            entry[startline+26] = dirent->ext[1];
            entry[startline+27] = dirent->ext[2];

            entry[startline+28] = ' ';
            entry[startline+29] = ' ';
            entry[startline+30] = ' ';
            entry[startline+31] = 0x00;
            file++;

            sendIEEEBytes(entry, 32, 0);
            /*
            if (dir->attrib != ATTR_VOLUME_ID)
            {
                dir_start += 0x0020;

                int startline = 0;
                fname_length = 0;

                entry[startline] = (unsigned char)(dir_start & 0x00ff);
                entry[startline+1] = (unsigned char)((dir_start & 0xff00) >> 8);
                entry[startline+2] = file+1;
                entry[startline+3] = 0x00;
                entry[startline+4] = 0x20;
                entry[startline+5] = 0x20;
                entry[startline+6] = 0x22;

                hasLongEntry = _filePosition.isLongFilename;
                if (hasLongEntry)
                {
                    if (_filePosition.fileName[0] == 0)
                    {
                        hasLongEntry = 0;
                    }
                }

                if (hasLongEntry)
                {
                    fname_length = strlen((char *)_filePosition.fileName);

                    if (fname_length > 5)
                    {
                        if (_filePosition.fileName[fname_length-4] == '.')
                        {
                            fname_length = fname_length-4;
                        }
                    }

                    if (fname_length >= 17)
                    {
                        fname_length = 17;
                    }

                    for (f = 0; f < fname_length; f++)
                    {
                        thisch = _filePosition.fileName[f];
                        if (thisch >= 'a' && thisch <= 'z')
                        {
                            thisch -= 32;
                        }
                        entry[startline+7+f] = thisch;
                    }
                }
                else
                {
                    fname_length = 0;
                    for (f = 0; f < 8; f++)
                    {
                        if (dir->name[f] == ' ')
                            break;

                        entry[startline+7+f] = dir->name[f];
                        fname_length++;
                    }
                }

                entry[startline+7+fname_length] = 0x22;
                for (f = 0; f < (17 - fname_length); f++)
                {
                    entry[startline+7+fname_length+f+1] = ' ';
                }


                if (dir->attrib == ATTR_DIRECTORY)
                {
                    entry[startline+25] = 'D';
                    entry[startline+26] = 'I';
                    entry[startline+27] = 'R';
                }
                else
                {
                    entry[startline+25] = dir->name[8];
                    entry[startline+26] = dir->name[9];
                    entry[startline+27] = dir->name[10];
                }

                entry[startline+28] = ' ';
                entry[startline+29] = ' ';
                entry[startline+30] = ' ';
                entry[startline+31] = 0x00;
                file++;

                sendIEEEBytes(entry, 32, 0);
            }
            */
        }
    }
    while (dirent != 0);
}

void writeFileFromIEEE(void* dataSource, unsigned char* buffer)
{
    int blocks = 0;
    unsigned int numBytes;
    unsigned char rdchar;
    unsigned char rdbus;
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

        if (numBytes >= 512)
        {
            // write buffer to datasource
            transmitString((unsigned char *)"writing block..\r\n");
            transmitHex(INT, blocks++);
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
        transmitString((unsigned char *)"writing last block\r\n");
        transmitHex(INT, blocks++);
        ds->writeBufferToFile(buffer, numBytes);
    }

    ds->closeFile();
    transmitString((unsigned char *)"done writing file.\r\n");
}
