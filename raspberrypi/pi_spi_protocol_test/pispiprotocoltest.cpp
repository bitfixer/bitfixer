#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include "timer.hpp"

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
    
    delayMicroseconds(100000);
    digitalWrite(resetPin, HIGH);
    
    printf("checking for commands_yo..\n");
    delayMicroseconds(500000);
    bool done = false;
    
    Tools::Timer timer;
    unsigned char pkt[1025];
    while (!done)
    {
        // loop - check for updates
        unsigned char cmd;
        printf("waiting for handshake.\n");
        
        while (digitalRead(0) == HIGH)
        {
            delayMicroseconds(10);
        }
        //printf("got handshake.\n");
        
        // get byte
        //int r = read(spi, &cmd, 1);
        //int r = read(spi, pkt, 10);
        
        int r = 0;
        timer.start();
        for (int i = 0; i < 1024; i++)
        {
            r += read(spi, &pkt[i], 1);
        }
        double t = timer.getTime();
        
        /*
        for (int x = 0; x < 10; x++)
        {
            int r = read(spi, &pkt[x], 1);
            printf("%d got %c %X\n", x, pkt[x], pkt[x]);
        }
        */
        
        
        //printf("got command: %d %02X\n", cmd, cmd);
        
        
        printf("got %d bytes: time %lf Bps %lf bps %lf\n", r, t, (double)r/t, (double)r*8.0/t);
        /*
        for (int b = 0; b < r; b++)
        {
            printf("byte %d: %d %X\n", b, pkt[b], pkt[b]);
        }
        */
        
        pkt[1024] = 0;
        printf("%s\n", pkt);
        
        FILE* fp = fopen("test.txt", "rb");
        fread(pkt, 1, 1024, fp);
        fclose(fp);
        
        timer.start();
        r = write(spi, pkt, 1024);
        t = timer.getTime();
        printf("sent %d bytes: time %lf Bps %lf bps %lf\n", r, t, (double)r/t, (double)r*8.0/t);
        
        
        printf("waiting for handshake high\n");
        // wait for deassert
        while (digitalRead(0) == LOW)
        {
            delayMicroseconds(10);
        }
        printf("handshake went high\n");
    }
    
    return 1;
}
