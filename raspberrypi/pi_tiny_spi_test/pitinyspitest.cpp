#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include "rpiSpiData.h"

// test - watch for input
int main(int argc, char **argv)
{
    //int resetPin = 3;
    int resetPin = 7;
    int spiReqPin = 4;
    int spiInterface = 1;
    
    wiringPiSetup();
    pinMode(spiReqPin, INPUT);
    pullUpDnControl(spiReqPin, PUD_UP);
    
    pinMode(resetPin, OUTPUT);
    pullUpDnControl(resetPin, PUD_OFF);
    digitalWrite(resetPin, LOW); // reset MCU

    int spi = wiringPiSPISetup(spiInterface, 500000);
    rpiSpiData spi_data(spiReqPin, spi);
    
    delayMicroseconds(100000);
    digitalWrite(resetPin, HIGH);

    printf("checking for commands_yo..\n");
    delayMicroseconds(100000);
    unsigned char pkt[1025];
    unsigned char size_bytes[2];
    
    while (1) {
        int recv_size = spi_data.receive(pkt);
        if (recv_size > 0)
        {
            printf("recv interface id %c %d %d %d bytes\n", pkt[0], pkt[0], pkt[1], recv_size);
            
            FILE* fp = fopen("test.txt", "rb");
            fread(pkt, 1, 256, fp);
            fclose(fp);
            
            spi_data.send(pkt, 10);
            printf("sent.\n");
            return 0;
        }
        delayMicroseconds(10);
    }

    /*
    while (1)
    {
        while (digitalRead(spiReqPin) == HIGH)
        {
            delayMicroseconds(10);
        }
        printf("got it.\n");
        
        for (int i = 0; i < 2; i++)
        {
            int r = read(spi, &pkt[i], 1);
        }
        
        for (int i = 0; i < 2; i++)
        {
            printf("got %c %d %X\n", pkt[i], pkt[i], pkt[i]);
        }
        
        // wait for deassert
        while (digitalRead(spiReqPin) != HIGH)
        {
            delayMicroseconds(10);
        }
        int r = write(spi, pkt, 1);
    }
    */

    return 1;
}
