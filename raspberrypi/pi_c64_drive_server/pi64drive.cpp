#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "timer.hpp"
#include "rpiSpiData.h"
#include "c64drive.h"
#include <thread>

#include "C64Dos.h"

#define MAX_BUF 1024

typedef enum
{
    Idle,
    GettingFilename,
    Saving,
    Loading
} driveState;

int main2(int argc, char **argv)
{
    int fd, fd_out;
    char * myfifo = "/tmp/c64drive";
    char * outfifo = "/tmp/spiserver";
    char buf[MAX_BUF];
    
    /* open, read, and display the message from the FIFO */
    fd = open(myfifo, O_RDONLY);
    fd_out = open(outfifo, O_WRONLY);
    printf("here\n");
    while (1)
    {
        int rb = read(fd, buf, MAX_BUF);
        printf("Received id %d %d\n", buf[0], buf[1]);
        //write(fd_out, "There", sizeof("There"));
        write(fd_out, buf, rb);
    }
    close(fd);
    close(fd_out);
    
    return 0;
}

void userInputThread()
{
    char tmp[256];
    while (1)
    {
        printf("reading..\n");
        int r = fscanf(stdin, "%s\n", tmp);
        printf("got %d : %s\n", r, tmp);
    }
}

// test - watch for input
int main(int argc, char **argv)
{
    /*
    // SPI 0
    int resetPin = 3; // wiringPi number
    // BCM GPIO 22
    int spiReqPin = 0;
    int spiInterface = 0;
    
    // SPI 1
    //int resetPin = 7; // wiringPi number
    // BCM GPIO 4
    //int spiReqPin = 4;
    //int spiInterface = 1;
    
    wiringPiSetup();
    pinMode(spiReqPin, INPUT);
    pullUpDnControl(spiReqPin, PUD_UP);

    pinMode(resetPin, OUTPUT);
    pullUpDnControl(resetPin, PUD_OFF);
    digitalWrite(resetPin, LOW); // reset MCU

    //int spi = wiringPiSPISetup(0, 2000000);
    int spi = wiringPiSPISetup(spiInterface, 2000000);
    rpiSpiData spi_data(spiReqPin, spi);

    delayMicroseconds(100000);
    digitalWrite(resetPin, HIGH);

    printf("C64 Drive started.\n");
    delayMicroseconds(100000);
    bool done = false;
    */
    
    int fd, fd_out;
    char * myfifo = "/tmp/c64drive";
    char * outfifo = "/tmp/spiserver";
    char buf[MAX_BUF];
    
    /* open, read, and display the message from the FIFO */
    fd = open(myfifo, O_RDONLY);
    fd_out = open(outfifo, O_WRONLY);

    Tools::Timer timer;
    unsigned char pkt[1025];
    unsigned char size_bytes[2];
    driveState state = Idle;
    
    char fname[256];
    int fname_len = 0;
    
    FILE* fp_load = NULL;
    int program_size = 0;
    int program_bytes_sent = 0;
    
    /*
    // test
    while (1)
    {
        int recv_size = spi_data.receive(pkt);
        if (recv_size > 0)
        {
            printf("recv id %d %d %d bytes\n", pkt[0], pkt[1], recv_size);
        }
        // send state response
        spi_data.send(pkt, 1);
    }
    */
    
    /*
    // TEST: from cbmdos
    dosInitDrives();
    dosMountDisk("POOLOFR0.D64", 0);
    
    CBMDOSChannel aChan;
    aChan.file = NULL;
    aChan.buffer = NULL;
    */
    C64Dos c64dos;
    c64dos.init();
    
    printf("C64 server\n");
    int channel = 0;
    
    /*
    fname[0] = '$';
    fname[1] = 0;
    
    aChan = dosOpenFile(fname, 0);
    if (!aChan.file)
    {
        printf("%s: file not found\n", fname);
    }
    
    printf("here\n");
    fseek(aChan.file, 0, SEEK_END);
    printf("yy\n");
    int dirsize = ftell(aChan.file);
    fseek(aChan.file, 0, SEEK_SET);
    printf("dir size %d\n", dirsize);
    
    return 1;
    */
    
    std::thread inputThread(userInputThread);
    
    /*
    while (1)
    {
        unsigned char tmp[128];
        int r = fread(tmp, 1, 10, stdin);
        if (r > 0)
        {
            printf("read: %s\n", tmp);
        }
        else
        {
            printf("*");
            delayMicroseconds(1000000);
        }
    }
    */
     
    while (1)
    {
        //int recv_size = spi_data.receive(pkt);
        int recv_size = read(fd, pkt, 1024);
        if (recv_size > 0)
        {
            dataPacket* dpkt = (dataPacket*)pkt;
            printf("recv id %d %d bytes data bytes %d atn bytes %d\n", dpkt->device_id, recv_size, dpkt->data_size, dpkt->atn_size);
            
            int total_bytes = dpkt->atn_size + dpkt->data_size;
            int atn_byte = 0;
            int data_byte = 0;
            while ((atn_byte + data_byte < total_bytes) || state == Loading)
            {
                // process packet
                if (state == Idle)
                {
                    if (atn_byte < dpkt->atn_size)
                    {
                        printf("IDLE ATN %d: %c %d %X\n", atn_byte, dpkt->atn_buffer[atn_byte], dpkt->atn_buffer[atn_byte], dpkt->atn_buffer[atn_byte]);
                        unsigned char b = dpkt->atn_buffer[atn_byte++];
                        if (b == 0xF1 || b == 0xF0)
                        {
                            // reading a filename
                            fname_len = 0;
                            state = GettingFilename;
                        }
                        else if (b == 0x61)
                        {
                            program_size = 0;
                            state = Saving;
                        }
                        else if (b >= 0x60 && b <= 0x6F)
                        {
                            printf("b = %X\n", b);
                            channel = b & 0x0F;
                            
                            if (b == 0x6F)
                            {
                                printf("atn_byte %d, atn_size %d\n", atn_byte, dpkt->atn_size);
                                for (int ab = atn_byte; ab < dpkt->atn_size; ab++)
                                {
                                    printf("atnbyte: %X\n", dpkt->atn_buffer[ab]);
                                }
                            }
                            
                            state = Loading;
                        }
                        else if (b == 0xE1 || b == 0xE0)
                        {
                            printf("Closing %s\n", fname);
                            c64dos.close();
                            fname_len = 0;
                            program_size = 0;
                        }
                    }
                    else if (data_byte < dpkt->data_size)
                    {
                        printf("IDLE %d: %c %d %X\n", data_byte, dpkt->data_buffer[data_byte], dpkt->data_buffer[data_byte], dpkt->data_buffer[data_byte]);
                        data_byte++;
                    }
                }
                else if (state == GettingFilename)
                {
                    if (data_byte < dpkt->data_size)
                    {
                        printf("FNAME %d: %c %d %X\n", data_byte, dpkt->data_buffer[data_byte], dpkt->data_buffer[data_byte], dpkt->data_buffer[data_byte]);
                        char b = dpkt->data_buffer[data_byte++];
                        fname[fname_len++] = b;
                    }
                    else if (atn_byte < dpkt->atn_size)
                    {
                        printf("FNAME ATN %d: %c %d %X\n", atn_byte, dpkt->atn_buffer[atn_byte], dpkt->atn_buffer[atn_byte], dpkt->atn_buffer[atn_byte]);
                        unsigned char b = dpkt->atn_buffer[atn_byte++];
                        if (b == UNLISTEN)
                        {
                            // reading a filename
                            state = Idle;
                        }
                    }
                }
                else if (state == Saving)
                {
                    if (dpkt->data_size > 0)
                    {
                        //if (!aChan.file)
                        if (!c64dos.fileIsOpen())
                        {
                            fname[fname_len] = 0;
                            if (!c64dos.open(fname, 1))
                            {
                                printf("error opening file %s for writing\n", fname);
                            }
                        }
                        
                        program_size += dpkt->data_size;
                        printf("Saving %s: %d bytes (%d)\n", fname, dpkt->data_size, program_size);
                        
                        c64dos.write(dpkt->data_buffer, dpkt->data_size);
                        data_byte += dpkt->data_size;
                    }
                    else if (atn_byte < dpkt->atn_size)
                    {
                        printf("SAVING ATN %d: %c %d %X\n", atn_byte, dpkt->atn_buffer[atn_byte], dpkt->atn_buffer[atn_byte], dpkt->atn_buffer[atn_byte]);
                        unsigned char b = dpkt->atn_buffer[atn_byte++];
                        if (b == UNLISTEN)
                        {
                            // reading a filename
                            state = Idle;
                        }
                    }
                    
                }
                else if (state == Loading)
                {
                    // load next chunk of data into the packet
                    if (!c64dos.fileIsOpen())
                    {
                        fname[fname_len] = 0;
                        
                        printf("loading channel %d\n", channel);
                        if (!c64dos.open(fname, channel))
                        {
                            printf("file not found error.\n");
                        }
                    }
                    
                    if (c64dos.fileIsOpen())
                    {
                        bool last = false;
                        // read from open file
                        int bytes_read = c64dos.read(dpkt->data_buffer, 128, last);
                        
                        dpkt->data_size = (unsigned char)bytes_read;
                        
                        int sent = c64dos.getFileBytesSent();
                        int length = c64dos.getFileLength();
                        
                        printf("Loading %s: %d / %d (%f %%)\n",
                               fname,
                               sent,
                               length,
                               (float)sent*100.0 / (float)length);
                        
                        if (last)
                        {
                            printf("last\n");
                            dpkt->is_last_data_buffer = 1;
                            program_bytes_sent = 0;
                            program_size = 0;
                            
                            c64dos.close();
                            state = Idle;
                        }
                        else
                        {
                            printf("not last\n");
                            dpkt->is_last_data_buffer = 0;
                        }
                    }
                    
                    break;
                }
            }
            
            // send state response
            write(fd_out, pkt, sizeof(dataPacket));
        }
    }

    return 1;
}
