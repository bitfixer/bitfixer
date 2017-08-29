#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "timer.hpp"
#include "rpiSpiData.h"

typedef struct
{
    int resetPin;
    int spiReqPin;
    int spiInterface;
    rpiSpiData* spi_data;
} spiInfo;

int main2(int argc, char **argv)
{
    int fd, fd_in;
    char * myfifo = "/tmp/c64drive";
    //char * infifo = "/tmp/spiserver";
    char buf[1024];
    
    mkfifo(myfifo, 0666);
    //mkfifo(infifo, 0666);
    
    fd = open(myfifo, O_WRONLY);
    //fd_in = open(infifo, O_RDONLY);
    
    write(fd, "Hi", sizeof("Hi"));
    
    //read(fd_in, buf, 1024);
    //printf("received %s\n", buf);
    close(fd);
    //close(fd_in);
    
    unlink(myfifo);
    
    return 0;
}

/*
typedef struct {
    int fd_out;
    int fd_in;
    char in_fifo[256];
    char out_fifo[256];
};
*/

class Fifo {
public:
    
    Fifo() {};
    ~Fifo() {};
    
    void init(const char* outName, const char* inName)
    {
        mkfifo(outName, 0666);
        mkfifo(inName, 0666);
        _fdOut = open(outName, O_WRONLY);
        _fdIn = open(inName, O_RDONLY);
    }
    
    void send(unsigned char* data, int size)
    {
        write(_fdOut, data, size);
    }
    
    int recv(unsigned char* data, int size)
    {
        return read(_fdIn, data, size);
    }
    
private:
    int _fdOut;
    int _fdIn;
};


// test - watch for input
int main(int argc, char **argv)
{
    printf("yo\n");
    //return 0;
    /*
    int fd, fd_in;
    char * myfifo = "/tmp/c64drive";
    char * infifo = "/tmp/spiserver";
    mkfifo(myfifo, 0666);
    mkfifo(infifo, 0666);
    fd = open(myfifo, O_WRONLY);
    fd_in = open(infifo, O_RDONLY);
    */
    
    Fifo fifos[2];
    //fifos[0].init("/tmp/pipix_out", "/tmp/pipix_in");
    fifos[1].init("/tmp/c64drive", "/tmp/spiserver");
    
    
    spiInfo spiInfo[2];
    spiInfo[0].resetPin = 3;
    spiInfo[0].spiReqPin = 0;
    spiInfo[0].spiInterface = 0;
    
    spiInfo[1].resetPin = 7;
    spiInfo[1].spiReqPin = 4;
    spiInfo[1].spiInterface = 1;
     
    printf("here.\n");
    
    wiringPiSetup();
    
    for (int i = 0; i < 2; i++)
    {
        pinMode(spiInfo[i].spiReqPin, INPUT);
        pullUpDnControl(spiInfo[i].spiReqPin, PUD_UP);
        
        pinMode(spiInfo[i].resetPin, OUTPUT);
        pullUpDnControl(spiInfo[i].resetPin, PUD_OFF);
        digitalWrite(spiInfo[i].resetPin, LOW); // reset this device
        
        int spi = wiringPiSPISetup(spiInfo[i].spiInterface, 2000000);
        spiInfo[i].spi_data = new rpiSpiData(spiInfo[i].spiReqPin, spi);
    }
    
    delayMicroseconds(100000);
    
    for (int i = 0; i < 2; i++)
    {
        digitalWrite(spiInfo[i].resetPin, HIGH);
    }

    printf("SPI server started.\n");
    delayMicroseconds(100000);
    unsigned char pkt[1025];
    
    // test
    while (1)
    {
        for (int i = 0; i < 2; i++)
        {
            int recv_size = spiInfo[i].spi_data->receive(pkt);
            if (recv_size > 0)
            {
                printf("recv interface %d id %d %d %d bytes\n", i, pkt[0], pkt[1], recv_size);
                
                if (i == 1)
                {
                    printf("writing to pipe\n");
                    //write(fd, pkt, recv_size);
                    //recv_size = read(fd_in, pkt, 1024);
                    fifos[i].send(pkt, recv_size);
                    fifos[i].recv(pkt, 1024);
                    
                    printf("recv %d bytes\n", recv_size);
                }
                
                spiInfo[i].spi_data->send(pkt, recv_size);
            }
        }
        
        delayMicroseconds(10);
    }

    /*
    while (1)
    {
        int recv_size = spi_data.receive(pkt);
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
                        else if (b == 0x60)
                        {
                            state = Loading;
                        }
                        else if (b == 0x61)
                        {
                            program_size = 0;
                            state = Saving;
                        }
                        else if (b == 0xE1)
                        {
                            printf("Closing %s\n", fname);
                            fclose(fp_load);
                            fp_load = NULL;
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
                    if (atn_byte < dpkt->atn_size)
                    {
                        printf("FNAME ATN %d: %c %d %X\n", atn_byte, dpkt->atn_buffer[atn_byte], dpkt->atn_buffer[atn_byte], dpkt->atn_buffer[atn_byte]);
                        unsigned char b = dpkt->atn_buffer[atn_byte++];
                        if (b == UNLISTEN)
                        {
                            // reading a filename
                            state = Idle;
                        }
                    }
                    else if (data_byte < dpkt->data_size)
                    {
                        printf("FNAME %d: %c %d %X\n", data_byte, dpkt->data_buffer[data_byte], dpkt->data_buffer[data_byte], dpkt->data_buffer[data_byte]);
                        char b = dpkt->data_buffer[data_byte++];
                        fname[fname_len++] = b;
                    }
                }
                else if (state == Saving)
                {
                    if (dpkt->data_size > 0)
                    {
                        if (!fp_load)
                        {
                            fname[fname_len] = 0;
                            fp_load = fopen(fname, "wb");
                        }
                        
                        program_size += dpkt->data_size;
                        printf("Saving %s: %d bytes (%d)\n", fname, dpkt->data_size, program_size);
                        fwrite(dpkt->data_buffer, 1, dpkt->data_size, fp_load);
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
                    if (!fp_load)
                    {
                        fname[fname_len] = 0;
                        fp_load = fopen(fname, "rb");
                        // get size
                        fseek(fp_load, 0, SEEK_END);
                        program_size = ftell(fp_load);
                        fseek(fp_load, 0, SEEK_SET);
                    }
                    
                    int bytes_read = (int)fread(dpkt->data_buffer, 1, 128, fp_load);
                    dpkt->data_size = (unsigned char)bytes_read;
                    program_bytes_sent += bytes_read;
                    printf("Loading %s: %d / %d (%f %%)\n", fname, program_bytes_sent, program_size, (float)program_bytes_sent*100.0 / (float)program_size);
                    
                    if (program_bytes_sent >= program_size)
                    {
                        dpkt->is_last_data_buffer = 1;
                        program_bytes_sent = 0;
                        program_size = 0;
                        fclose(fp_load);
                        fp_load = NULL;
                        state = Idle;
                    }
                    else
                    {
                        dpkt->is_last_data_buffer = 0;
                    }
                    
                    break;
                }
            }
            
            // send state response
            spi_data.send(pkt, sizeof(dataPacket));
        }
    }
     */
    return 1;
}
