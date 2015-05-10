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
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>
#include "PETdisk.h"
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

// addresses for PET IEEE commands
#define PET_LOAD_FNAME_ADDR     0xF0
#define PET_SAVE_FNAME_ADDR     0xF1
#define PET_OPEN_FNAME_MASK     0xF0

// macro for checking if line is low
#define SIGNAL_IS_LOW(A, B)     (A & B) == 0x00

typedef enum _pdstate
{
    IDLE,
    BUS_LISTEN,
    BUS_TALK,
    LOAD_FNAME_READ,
    SAVE_FNAME_READ,
    OPEN_FNAME_READ,
    OPEN_FNAME_READ_DONE,
    OPEN_DATA_WRITE,
    OPEN_DATA_WRITE_DONE,
    OPEN_DATA_READ,
    DIR_READ,
    FILE_READ_OPENING,
    FILE_SAVE_OPENING,
    FILE_READ,
    FILE_SAVE,
    OPEN_FNAME,
    FILE_NOT_FOUND,
    CLOSING
} pdstate;

typedef enum _filedir
{
    FNONE,
    FREAD,
    FWRITE
} filedir;

typedef enum { false, true } bool;

typedef struct _pdStateVars
{
    char openFileAddress;
    int fileWriteByte;
    unsigned int fileReadByte;
    unsigned char useRemainderByte;
    unsigned char remainderByte;
    filedir fileDirection;
    unsigned char fileNotFound;
    unsigned long currentDirectoryCluster;
    unsigned char sd_boot_checksum;
} pdStateVars;

const unsigned char _dirHeader[] PROGMEM =
{
    0x01,
    0x04,
    0x1F,
    0x04,
    0x00,
    0x00,
    0x12,
};

const unsigned char _versionString[] PROGMEM = "\"PETDISK V3.0b   \"      ";

const unsigned char _saving[] PROGMEM = "Saving";
const unsigned char _reading[] PROGMEM = "Reading";
const unsigned char _nofirmware[] PROGMEM = "No Firmware.";

const unsigned char _fileExtension[] PROGMEM =
{
    '.',
    'P',
    'R',
    'G',
    0x00,
};

const unsigned char _seqExtension[] PROGMEM =
{
    '.',
    'S',
    'E',
    'Q',
    0x00,
};

const unsigned char _firmwareFileName[] PROGMEM =
{
    'F',
    'I',
    'R',
    'M',
    '*',
    0x00,
};

void pgm_memcpy(unsigned char *dest, unsigned char *src, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        *dest++ = pgm_read_byte(&(*src++));
    }
}

void port_init(void)
{
    SPI_CTL = (unsigned char)(~MISO & ~DATA0 & ~DATA1 & ~CASSETTE_READ & ~CASSETTE_WRITE);
    SPI_PORT = 0x7f;

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

unsigned char processFilename(unsigned char *filename, unsigned char length)
{
    unsigned char i,j;
    unsigned char pLength = length;
    // look for :, indicating drive number
    for (i = 0; i < length; i++)
    {
        if (filename[i] == ':')
        {
            break;
        }
    }
    
    if (i < length)
    {
        i++;
        // move string down
        pLength -= i;
        
        for (j = 0; j < pLength; j++)
        {
            filename[j] = filename[j+i];
        }
    }
    
    for (i = 0; i < pLength; i++)
    {
        if (filename[i] == ',')
        {
            break;
        }
    }
    
    pLength = i;
    return pLength;
}

// initialize SD card
// note: this reads into global _buffer. Will trash any data there
unsigned char initializeSDCard(pdStateVars *stateVars)
{
    int i;
    unsigned char error;
    unsigned char chksum = 0;
    // initialize card
    for (i = 0; i < 10; i++)
    {
        error = SD_init();
        if (!error)
            break;
    }
    
    if (i == 10)
    {
        // reset current directory to root
        stateVars->currentDirectoryCluster = 0;
    }
    
    error = getBootSectorData (); //read boot sector and keep necessary data in global variables
    
    // check for card change
    // calculate checksum
    for (i = 0; i < 512; i++)
    {
        chksum += _buffer[i];
    }
    
    if (chksum != stateVars->sd_boot_checksum)
    {
        stateVars->sd_boot_checksum = chksum;
        stateVars->currentDirectoryCluster = 0;
    }
    
    if (stateVars->currentDirectoryCluster == 0)
    {
        stateVars->currentDirectoryCluster = _rootCluster;
    }
    
    return error;
}

int main(void)
{
    unsigned char progname[FNAMELEN];
    unsigned char rdchar,rdbus;
    unsigned char error;
    unsigned char filename_position;
    unsigned char address;
    unsigned int bytes_to_send;
    unsigned char i;
    unsigned char doneSending;
    
    // initialize state variables
    pdStateVars stateVars;
    stateVars.openFileAddress = -1;
    stateVars.fileWriteByte = -1;
    stateVars.fileDirection = FNONE;
    stateVars.currentDirectoryCluster = 0;
    stateVars.sd_boot_checksum = 0;
    stateVars.remainderByte = 0;
    stateVars.useRemainderByte = 0;
    
    address = get_device_address();
    
    init_devices();
    
    _cardType = 0;

    struct dir_Structure *dir;
    
    unsigned char buscmd;
    pdstate currentState = IDLE;
    
    filename_position = 0;
    
    // clear string
    memset(progname, 0, FNAMELEN);
    
    error = initializeSDCard(&stateVars);
    if (!error)
    {
        // copy firmware filename
        pgm_memcpy((unsigned char *)progname, (unsigned char *)_firmwareFileName, 5);
        dir = findFile(progname, _rootCluster);
        
        if (dir != 0)
        {
            // found firmware file
            deleteFile();
        }
        else
        {
            transmitString_F(_nofirmware);
        }
    }
     
    // start main loop
    while(1)
    {
        if (currentState == FILE_NOT_FOUND)
        {
            unlisten();
            currentState = IDLE;
            stateVars.fileNotFound = 0;
            filename_position = 0;
            memset(progname, 0, FNAMELEN);
        }
        
        if (IEEE_CTL == 0x00)
        {
            //SPI_PORT = 0xFF;
            // if we are in an unlisten state,
            // wait for my address
            buscmd = wait_for_device_address(address);
            if (buscmd == LISTEN)
            {
                currentState = BUS_LISTEN;
            }
            else
            {
                currentState = BUS_TALK;
            }
        }
    
        wait_for_dav_low();
        
        // lower NDAC and NRFD
        PORTC = NOT_NDAC & NOT_NRFD;
        
        // read byte
        recv_byte_IEEE(&rdchar);
        
        // read bus value
        rdbus = PINC;
        
        if ((rdbus & ATN) == 0x00) // check for bus command
        {
            //transmitHex(CHAR, rdchar);
            if (rdchar == 0xF0)
            {
                currentState = LOAD_FNAME_READ;
            }
            else if (rdchar == 0xF1)
            {
                currentState = SAVE_FNAME_READ;
            }
            else if ((rdchar & 0xF0) == 0xF0) // open command to another address
            {
                currentState = OPEN_FNAME_READ;
                stateVars.openFileAddress = (rdchar & 0x0F);
                stateVars.fileWriteByte = -1;
            }
            else if (rdchar == 0x60) // read command
            {
                if (stateVars.fileNotFound == 1)
                {
                    currentState = FILE_NOT_FOUND;
                }
                else
                {
                    //transmitString("$");
                    currentState = FILE_READ;
                    // open file for reading
                    if (progname[0] == '$')
                    {
                        // copy the directory header
                        pgm_memcpy((unsigned char *)_buffer, (unsigned char *)_dirHeader, 7);
                        
                        // print directory title
                        pgm_memcpy((unsigned char *)&_buffer[7], (unsigned char *)_versionString, 24);
                        _buffer[31] = 0x00;
                    }
                }
            }
            else if (rdchar == 0x61) // save command
            {
                currentState = FILE_SAVE;
            }
            else if ((rdchar & 0xF0) == 0x60) // print or input command
            {
                unsigned char temp = rdchar & 0x0F;
                if (temp == stateVars.openFileAddress)
                {
                    if (currentState == BUS_LISTEN)
                    {
                        if (stateVars.fileWriteByte == -1)
                        {
                            transmitString_F(_saving);
                            openFileForWriting(progname, stateVars.currentDirectoryCluster);
                            stateVars.fileWriteByte = 0;
                        }
                        stateVars.fileDirection = FWRITE;
                        currentState = OPEN_DATA_WRITE;
                    }
                    else
                    {
                        if (stateVars.fileNotFound == 1)
                        {
                            stateVars.fileDirection = FNONE;
                            currentState = FILE_NOT_FOUND;
                        }
                        else
                        {
                            stateVars.fileDirection = FREAD;
                            currentState = OPEN_DATA_READ;
                        }
                    }
                }
            }
            //else if (rdchar == 0xE2) // close command
            else if ((rdchar & 0xF0) == 0xE0)
            {
                unsigned char temp = rdchar & 0x0F;
                if (temp == stateVars.openFileAddress && stateVars.fileDirection == FWRITE)
                {
                    if (stateVars.fileWriteByte > 0)
                    {
                        writeBufferToFile(stateVars.fileWriteByte);
                        stateVars.fileWriteByte = 0;
                    }
                    
                    closeFile();
                }
                
                stateVars.openFileAddress = -1;
                stateVars.fileWriteByte = -1;
                stateVars.fileDirection = FNONE;
                currentState = CLOSING;
            }
        }
        else if (currentState == OPEN_DATA_WRITE) // received byte to write to open file
        {
            _buffer[stateVars.fileWriteByte++] = rdchar;
            if (stateVars.fileWriteByte >= 512)
            {
                writeBufferToFile(stateVars.fileWriteByte);
                stateVars.fileWriteByte = 0;
            }
        }
        else if (currentState == LOAD_FNAME_READ ||
                 currentState == SAVE_FNAME_READ ||
                 currentState == OPEN_FNAME_READ)
        {
            // add character to filename
            progname[filename_position] = rdchar;
            filename_position++;
            progname[filename_position] = 0;
            
            if ((rdbus & EOI) == 0)
            {
                // this is a directory request
                if (progname[0] == '$')
                {
                    filename_position = 0;
                    currentState = DIR_READ;
                }
                else 
                {
                    // process filename, remove drive indicators and file type
                    filename_position = processFilename(progname, filename_position);
                
                    unsigned char *ext;
                    if (currentState == OPEN_FNAME_READ)
                    {
                        ext = _seqExtension;
                        //currentState = IDLE;
                        currentState = OPEN_FNAME_READ_DONE;
                    }
                    else
                    {
                        ext = _fileExtension;
                        
                        if (currentState == LOAD_FNAME_READ)
                        {
                            currentState = FILE_READ_OPENING;
                        }
                        else if (currentState == SAVE_FNAME_READ)
                        {
                            currentState = FILE_SAVE_OPENING;
                        }
                    }
                    
                    // copy the PRG file extension onto the end of the file name
                    pgm_memcpy(&progname[filename_position], ext, 5);
                    filename_position = 0;
                    transmitString(progname);
                }
            }
        }
        
        // raise NDAC
        PORTC = NOT_NRFD;
        wait_for_dav_high();
        // open file if needed
        
        // === PREPARE FOR READ/WRITE
        // === re-init sd card and open file
        
        if (currentState == FILE_READ_OPENING ||
            currentState == FILE_SAVE_OPENING ||
            currentState == OPEN_FNAME_READ_DONE ||
            currentState == DIR_READ)
        {
            // initialize sd card
            error = initializeSDCard(&stateVars);
            
            if (currentState == FILE_SAVE_OPENING)
            {
                // open file
                openFileForWriting(progname, stateVars.currentDirectoryCluster);
                currentState = IDLE;
            }
            else if (currentState == FILE_READ_OPENING ||
                     currentState == OPEN_FNAME_READ_DONE) // file read, either LOAD or OPEN command
            {
                if (!openFileForReading(progname, stateVars.currentDirectoryCluster))
                {
                    // file not found
                    stateVars.fileNotFound = 1;
                }
                else
                {
                    if (currentState == OPEN_FNAME_READ_DONE)
                        bytes_to_send = getNextFileBlock();
                    stateVars.fileReadByte = 0;
                    stateVars.fileNotFound = 0;
                    stateVars.useRemainderByte = 0;
                    stateVars.remainderByte = 0;
                }
                
                currentState = IDLE;
            }
        }
        
        if ((rdchar == UNLISTEN) || (rdchar == UNTALK && (rdbus & ATN) == 0x00))
        {
            // unlisten or untalk command
            PORTC = NOT_NDAC;
            unlisten();
            currentState = IDLE;
        }
        else 
        {
            // lower NRFD
            PORTC = NOT_NDAC;
        }
        
        // LOAD requested
        if (currentState == FILE_READ || currentState == OPEN_DATA_READ)
        {
            // ==== STARTING LOAD SEQUENCE
            
            // release NRFD/NDAC
            DDRC = NDAC;
            
            // wait for atn high
            wait_for_atn_high();
            
            DDRC = DAV | EOI;
            PORTC = 0xFF;
            
            // change data bus to output
            DATA_CTL = 0xff;
            DDRB = DDRB | (DATA0 | DATA1);
            
            wait_for_ndac_low();
             
            if (currentState == FILE_READ)
            {
                // get packet
                if (progname[0] == '$')
                {
                    //transmitString("@");
                    sendIEEEBytes((unsigned char *)_buffer, 32, 0);
                    
                    // this is a change directory command
                    if (progname[1] == ':')
                    {
                        // check if we should return to root
                        if ((progname[2] == '\\' || progname[2] == '/') && progname[3] == 0)
                        {
                            stateVars.currentDirectoryCluster = _rootCluster;
                        }
                        else
                        {
                            // get the cluster for the new directory
                            dir = findFile(&progname[2], stateVars.currentDirectoryCluster);
                            
                            if (dir != 0)
                            {
                                // get new directory cluster
                                stateVars.currentDirectoryCluster = getFirstCluster(dir);
                                if (stateVars.currentDirectoryCluster == 0)
                                {
                                    stateVars.currentDirectoryCluster = _rootCluster;
                                }
                            }
                        }
                    }
                    
                    // write directory entries
                    ListFilesIEEE(stateVars.currentDirectoryCluster);
                }
                else // read from file
                {
                    transmitString_F(_reading);
                    // send blocks of file
                    doneSending = 0;
                    while(doneSending == 0)
                    {
                        bytes_to_send = getNextFileBlock();
                        if (_filePosition.byteCounter >= _filePosition.fileSize)
                        {
                            doneSending = 1;
                        }
                        
                        sendIEEEBytes((unsigned char *)_buffer, bytes_to_send, doneSending);
                    }
                }
            }
            else if (currentState == OPEN_DATA_READ)
            {
                bool done = false;
                bool found = false;
                unsigned char temp = 0;
                unsigned char result = 0;
                
                while (!done)
                {
                    if (stateVars.useRemainderByte == 1)
                        result = sendIEEEByteCheckForATN(stateVars.remainderByte);
                    else
                        result = sendIEEEByteCheckForATN(_buffer[stateVars.fileReadByte]);

                    result = wait_for_ndac_high_or_atn_low();
                    
                    if (result == ATN)
                    {
                        done = true;
                    }
                    else
                    {
                        if (stateVars.useRemainderByte == 1)
                        {
                            stateVars.useRemainderByte = 0;
                        }
                        else
                        {
                            stateVars.remainderByte = _buffer[stateVars.fileReadByte];
                            stateVars.fileReadByte++;
                        }
                        
                        if (stateVars.fileReadByte >= 512)
                        {
                            // get next buffer block
                            bytes_to_send = getNextFileBlock();
                            stateVars.fileReadByte = 0;
                        }
                        
                        // raise DAV
                        temp = DAV | EOI;
                        // output to bus
                        PORTC = temp;
                     
                        result = wait_for_ndac_low_or_atn_low();
                        
                        if (result == ATN)
                        {
                            if (stateVars.fileReadByte == 0)
                            {
                                stateVars.useRemainderByte = 1;
                            }
                            else
                            {
                                stateVars.fileReadByte--;
                            }
                            
                            done = true;
                        }
                    }
                }
            }
        
            // ==== ENDING LOAD SEQUENCE
            
            // raise DAV and EOI
            PORTC = 0xFF;
            
            // switch back to input mode
            DDRC = NRFD | NDAC;
            
            DATA_CTL = 0x00;
            DDRB = (unsigned char)(~MISO & ~DATA0 & ~DATA1 & ~CASSETTE_READ & ~CASSETTE_WRITE);
            PORTC = NOT_NDAC;
            
            // ====
            
            unlisten();
            currentState = IDLE;
            
        }
        else if (currentState == FILE_SAVE)
        {
            // save command
            writeFileFromIEEE();
            unlisten();
            currentState = IDLE;
        }
    }
    
}