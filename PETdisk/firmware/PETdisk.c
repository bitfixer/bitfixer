/*
    PETdisk.c
    Main program for the PETdisk storage device
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

#define F_CPU 8000000UL		//freq 8 MHz
#define BAUD 19200
#define MYUBRR F_CPU/16/BAUD-1
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "SPI_routines.h"
#include "SD_routines.h"
#include "UART_routines.h"
#include "FAT32.h"
#include "IEEE488.h"

#define SPI_PORT PORTB
#define SPI_CTL  DDRB
#define MISO     0x10
#define CASSETTE_READ 0x40
#define CASSETTE_WRITE 0x80

#define FNAMELEN    39

void port_init(void)
{
SPI_CTL = ~MISO & ~DATA0 & ~DATA1 & ~CASSETTE_READ & ~CASSETTE_WRITE;
SPI_PORT = 0xff;

/*
IEEE_CTL = NDAC | NRFD;
PORTC = ~NDAC;
*/

// all IEEE lines input
IEEE_CTL = 0x00;
// activate pullups
IEEE_PORT = 0xFF;

// set data lines to input
DATA_CTL = 0x00;
}


//call this routine to initialize all peripherals
void init_devices(void)
{
 cli();  //all interrupts disabled
 port_init();
 spi_init();
 uart0_init(MYUBRR);

 MCUCR = 0x00;
 //GICR  = 0x00;
 //TIMSK = 0x00; //timer interrupt sources
 //all peripherals are now initialized
}

unsigned char get_device_address()
{
    DDRC = 0x00;
    DDRD = 0x00;
    
    // turn on pullup resistors
    PORTC = 0x20;
    PORTD = 0x01;
    
    _delay_ms(100);  //delay for VCC stabilization

    unsigned char addrlo,addrhi,addr;
    // read jumper settings
    addrlo = PIND;
    addrhi = PINC;
    
    // invert readings - jumper present should = 1 for that bit
    addrlo = ~addrlo;
    addrhi = ~addrhi;
    
    // bitmask and shift
    addrlo = addrlo & 0x01;
    addrhi = (addrhi & 0x20) >> 4;
    addr = addrhi | addrlo;
    
    addr = addr + 8;
    return addr;
    
}

int main(void)
{
    unsigned char fileName[11];
    unsigned char progname[FNAMELEN];
    unsigned char tmp[50];
    unsigned char rdchar,rdbus,tmp1,tmp2,ctl;
    unsigned char option, error, data, FAT32_active;
    unsigned char getting_filename;
    unsigned char filename_position;
    unsigned char islong;
    unsigned char address;
    int bytes_to_send;
    unsigned char i;
    
    address = get_device_address();
    
    init_devices();
    transmitString("Device ID: ");
    transmitHex(CHAR, address);
    TX_NEWLINE;
    
    cardType = 0;

    struct dir_Structure *dir;
    unsigned long cluster, byteCounter = 0, fileSize, firstSector;
    unsigned int k;
    unsigned char j,sending;
    unsigned char response;
    unsigned char startline;
    unsigned int retry;
    unsigned int dir_start;
    unsigned int dirlength;
    unsigned char gotname;
    unsigned char savefile;
    unsigned char filenotfound;
    unsigned char initcard;
    unsigned char buscmd;
    //unsigned long startBlock;
    unsigned long cl;
    getting_filename = 0;
    filename_position = 0;
    // clear string
    for (i = 0; i < FNAMELEN; i++)
    {
        progname[i] = 0x00;
    }
    
    // initialize SD card
    for (i=0; i<10; i++)
    {
      error = SD_init();
      if(!error) break;
    }
    
    if (!error)
    {
        error = getBootSectorData (); //read boot sector and keep necessary data in global variables
    
		transmitString("got card");
		transmitHex(INT, bytesPerSector);
	
        // look for firmware file
        progname[0] = 'F';
        progname[1] = 'I';
        progname[2] = 'R';
        progname[3] = 'M';
        progname[4] = '*';
        dir = findFilesL(GET_FILE, progname, 0);
        
        if (dir != 0)
        {
            // found firmware file
            transmitString("found firmware..");
            
            // delete the file
            dir = findFilesL(DELETE, progname, 0);
        } 
        else 
        {
            transmitString("no firmware.");
        }

    
    }   
    
    // start main loop
    while(1)
    {
        if (IEEE_CTL == 0x00)
        {
            // if we are in an unlisten state,
            // wait for my address
            //transmitString("waiting.");
            buscmd = wait_for_device_address(address);
            filenotfound = 0;
            if (buscmd == LISTEN)
            {
                initcard = 0;
            }
        }
    
        wait_for_dav_low();
        
        // lower NDAC and NRFD
        PORTC = NOT_NDAC & NOT_NRFD;
        
        // read byte
        recv_byte_IEEE(&rdchar);
        
        // read bus value
        rdbus = PINC;
        
        if (filenotfound == 1)
        {
            filenotfound = 0;
            unlisten();
        }
        else if ((rdchar == 0xf0 || rdchar == 0xf1) && (rdbus & ATN) == 0x00)
        {
            // we are retrieving a filename for a load or save
            getting_filename = 1;
            if (rdchar == 0xf1)
            {
                // this is a save
                savefile = 1;
            }
            else 
            {
                savefile = 0;
            }

        }
        else if (getting_filename == 1)
        {
            // add character to filename
            progname[filename_position] = rdchar;
            filename_position++;
            progname[filename_position] = 0;
            
            if ((rdbus & EOI) == 0)
            {
            
                if (progname[0] == '$')
                {
                    getting_filename = 0;
                    filename_position = 0;
                }
                else 
                {
                    // check for DLOAD command, remove 0: from start of filename
                    if (progname[0] == '0' && progname[1] == ':')
                    {
                        for (i = 0; i < filename_position-2; i++)
                        {
                            progname[i] = progname[i+2];
                        }
                        
                        filename_position -= 2;
                    }
                
                
                    // add extension
                    progname[filename_position++] = '.';
                    progname[filename_position++] = 'P';
                    progname[filename_position++] = 'R';
                    progname[filename_position++] = 'G';
                    progname[filename_position] = 0;
                
                    getting_filename = 0;
                    filename_position = 0;
                    
                    /*
                    // testing only
                    progname[filename_position++] = 'A';
                    progname[filename_position++] = '.';
                    progname[filename_position++] = 'P';
                    progname[filename_position++] = 'R';
                    progname[filename_position++] = 'G';
                    progname[filename_position] = 0;
                    */
                    
                    transmitString(progname);
                    
                    gotname = 1;
                }
            }
        }
        else if (rdchar == 0x60 && (rdbus & ATN) == 0x00)
        {
            // try reading block
            if (progname[0] == '$')
            {
                buffer[0] = 0x01;
                buffer[1] = 0x04;
                buffer[2] = 0x1F;
                buffer[3] = 0x04;
                buffer[4] = 0x00;
                buffer[5] = 0x00;
                buffer[6] = 0x12;
                
                // print directory title
                sprintf(&buffer[7], "\"PETDISK V1.21   \"      ");
                buffer[31] = 0x00;
            }
        }
        
        // raise NDAC
        PORTC = NOT_NRFD;
        wait_for_dav_high();
        // open file if needed
        if (initcard == 0)
        {
            //transmitString("init card");
            // initialize card
            for (i=0; i<10; i++)
            {
              error = SD_init();
              if(!error) break;
            }
            
            error = getBootSectorData (); //read boot sector and keep necessary data in global variables
            
            //transmitString("@");
            initcard = 1;
        }
        if (gotname == 1)
        {
            if (savefile == 0)
            {
                islong = convertFileName(progname);
                    
                dir = findFilesL(GET_FILE, progname, islong);
                if (dir == 0)
                {
                    // file not found
                    // release the bus
                    transmitString("file not found");
                    //unlisten();
                    filenotfound = 1;
                }
                else
                {
                    cluster = (((unsigned long) dir->firstClusterHI) << 16) | dir->firstClusterLO;
                    fileSize = dir->fileSize;
                    firstSector = getFirstSector (cluster);
                    SD_readSingleBlock(firstSector);
                    //transmitString("got file!");
                }
                
                // clear string
                for (i = 0; i < FNAMELEN; i++)
                {
                    progname[i] = 0x00;
                }
            }
            else 
            {
                // open file
                openFile(progname, &cl);
                cl = fileStartCluster;
            }

        }
        
        gotname = 0;
        
        //transmitString(".");
        
        if ((rdchar == 0x3f) || rdchar == 0x5f && (rdbus & ATN) == 0x00)
        {
            // unlisten or untalk command
            //unlisten();
            PORTC = NOT_NDAC;
            transmitByte('*');
            transmitHex(CHAR, rdchar);
            
            unlisten();
        }
        else 
        {
            // lower NRFD
            PORTC = NOT_NDAC;
        }
        
        // LOAD requested
        if (rdchar == 0x60 && (rdbus & ATN) == 0x00)
        {
            if (filenotfound == 0)
            {
            // this is a LOAD
            // release NRFD/NDAC
            DDRC = NDAC;
            
            // wait for atn high
            wait_for_atn_high();
        
            DDRC = DAV | EOI;
            PORTC = 0xFF;
            
            // change data bus to output
            //DDRA = 0xFF;
            DATA_CTL = 0xff;
            DDRB = DDRB | (DATA0 | DATA1);
            
            // get packet
            
            if (strcmp(progname, "$") == 0)
            {
                transmitString("directory..");
                for (i = 0; i < 32; i++)
                {
                    send_byte(buffer[i],0);
                }
                
                // write directory entries
                ListFilesIEEE();
            }
            else
            {
            sending = 1;
            byteCounter = 0;
            while(sending == 1)
            {
              firstSector = getFirstSector (cluster);
            
              for(j=0; j<sectorPerCluster; j++)
              {
                if (byteCounter > 0)
                {
                    SD_readSingleBlock(firstSector + j);
                }
                
                for(k=0; k<512; k++)
                {
                    if (byteCounter+1 >= fileSize)
                    {
                        send_byte(buffer[k], 1);
                        k = 512;
                        j = sectorPerCluster;
                        sending = 0;
                    }
                    else
                    {
                        send_byte(buffer[k], 0);
                        byteCounter++;
                    }
                }
              }
              cluster = getSetNextCluster (cluster, GET, 0);
              //if(cluster == 0) {transmitString_F(PSTR("Error in getting cluster")); return 0;}
            }
            }
            
            // raise DAV and EOI
            PORTC = 0xFF;
            
            // switch back to input mode
            
            DDRC = NRFD | NDAC;
            
            DATA_CTL = 0x00;
            DDRB = ~MISO & ~DATA0 & ~DATA1 & ~CASSETTE_READ & ~CASSETTE_WRITE;
            PORTC = NOT_NDAC;
            }
            
            unlisten();
            
        }
        else if (rdchar == 0x61 && (rdbus & ATN) == 0)
        {
            // save command
            writeFileFromIEEE(progname, cl);
            unlisten();
        }
    }
    
}