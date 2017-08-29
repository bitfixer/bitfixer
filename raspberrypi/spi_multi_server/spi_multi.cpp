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
#include "Fifo.h"

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

// test - watch for input
int main(int argc, char **argv)
{
    printf("yo\n");
    Fifo fifos[2];
    fifos[0].init("/tmp/pipix_out", "/tmp/pipix_in", true);
    fifos[1].init("/tmp/c64drive", "/tmp/spiserver", true);
    
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
                
                /*
                if (i == 1)
                {
                    printf("writing to pipe\n");
                    fifos[i].send(pkt, recv_size);
                    fifos[i].recv(pkt, 1024);
                    printf("recv %d bytes\n", recv_size);
                }
                */
                
                printf("writing to pipe\n");
                fifos[i].send(pkt, recv_size);
                recv_size = fifos[i].recv(pkt, 1024);
                printf("recv %d bytes\n", recv_size);
                
                spiInfo[i].spi_data->send(pkt, recv_size);
            }
        }
        
        delayMicroseconds(10);
    }
    return 1;
}
