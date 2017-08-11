#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include "timer.hpp"
#include "rpiSpiData.h"

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
            pkt[recv_size] = 0;
            //printf("%d bytes: %s\n", recv_size, pkt);

            for (int i = 0; i < recv_size; i++)
            {
                printf("%d: %c %d %X\n", i, pkt[i], pkt[i], pkt[i]);
            }
            
            /*
            FILE* fp = fopen("test.txt", "rb");
            fread(pkt, 1, 1024, fp);
            fclose(fp);

            int send_size = 1024;
            spi_data.send(pkt, send_size);
            printf("sent %d bytes.\n", send_size);
            */
            
            pkt[0] = 'A';
            int send_size = 1;
            spi_data.send(pkt, 1);
            //printf("sent %d bytes.\n", send_size);
        }
        else
        {
            printf("gotit.\n");
            //delayMicroseconds(500);
            done = true;
        }
    }

    return 1;
}
