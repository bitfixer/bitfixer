#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include "timer.hpp"
#include "rpiSpiData.h"

typedef struct
{
    unsigned char state;
    unsigned char ss;
    bool isAtn;
    unsigned char size;
    unsigned char buffer[256];
    unsigned char atn_size;
    unsigned char atn_buffer[64];
} dataPacket;

typedef enum
{
    WaitingForAtn = 0,
    ReadingAtn = 1,
    Listening = 2,
    Talking = 3,
    Unlisten = 4
} DeviceState;

// test - watch for input
int main(int argc, char **argv)
{
    int resetPin = 3;

    wiringPiSetup();
    pinMode(0, INPUT);
    pullUpDnControl(0, PUD_UP);

    pinMode(resetPin, OUTPUT);
    pullUpDnControl(resetPin, PUD_OFF);
    digitalWrite(resetPin, LOW); // reset MCU

    int spi = wiringPiSPISetup(0, 2000000);
    rpiSpiData spi_data(0, spi);

    delayMicroseconds(100000);
    digitalWrite(resetPin, HIGH);

    printf("checking for commands_yo..\n");
    delayMicroseconds(100000);
    bool done = false;

    Tools::Timer timer;
    unsigned char pkt[1025];
    unsigned char size_bytes[2];

    while (!done)
    {
        int recv_size = spi_data.receive(pkt);
        if (recv_size > 0)
        {
            dataPacket* dpkt = (dataPacket*)pkt;
            //printf("ATN: %d\n", dpkt->isAtn);
            //printf("state: %d %d\n", dpkt->state, dpkt->ss);
            //printf("Size: %d\n", dpkt->size);
            
            for (int i = 0; i < dpkt->atn_size; i++)
            {
                printf("ATN %d: %c %d %X\n", i, dpkt->atn_buffer[i], dpkt->atn_buffer[i], dpkt->atn_buffer[i]);
            }
            
            for (int i = 0; i < dpkt->size; i++)
            {
                printf("%d: %c %d %X\n", i, dpkt->buffer[i], dpkt->buffer[i], dpkt->buffer[i]);
            }
            
            unsigned char cmd = dpkt->buffer[dpkt->size-1];
            if (cmd == 0xF1)
            {
                //printf("Now Listening.\n");
                dpkt->state = Listening;
            }
            else if (cmd == 0x61)
            {
                //printf("Now Listening.\n");
                dpkt->state = Listening;
            }
            else if (cmd == 0x3F)
            {
                //printf("Unlisten.\n");
                dpkt->state = Unlisten;
            }
            else
            {
                //printf("Unlisten.\n");
                dpkt->state = Unlisten;
            }
            
            // send state response
            spi_data.send(pkt, 1);
        }
        /*
        else
        {
            printf("gotit.\n");
            //delayMicroseconds(500);
            done = true;
        }
        */
    }

    return 1;
}
