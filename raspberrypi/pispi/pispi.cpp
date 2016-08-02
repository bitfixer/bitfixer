#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>

// test - watch for input
int main(int argc, char **argv)
{
    unsigned char buffer[1024];
    for (int i = 0; i < 1024; i++)
    {
        int c = i % 40;
        int l = c % 26;
        buffer[i] = 'A' + l;
    }
    
    wiringPiSetup();
    pinMode(0, INPUT);
    pullUpDnControl(0, PUD_OFF);
    
    int spi = wiringPiSPISetup(0, 2000000);
    
    printf("checking for commands..\n");
    unsigned char mapchar = 1;
    
    while (1)
    {
        // loop - check for updates
        unsigned char cmd;
        //int r = read(spi, &cmd, 1);
        
        printf("waiting for command\n");
        while (fastDigitalRead(0) == HIGH)
        {
            delayMicroseconds(10);
        }
        printf("reading command\n");
        
        // get byte
        int r = read(spi, &cmd, 1);
        printf("got %d %02X\n", cmd, cmd);
        
        if (cmd == 0)
        {
            for (int i = 0; i < 1024; i++)
            {
                //int c = i % 40;
                //int l = (c + cmd) % 26;
                //buffer[i] = 'A' + l;
                buffer[i] = 0x01;
            }

            mapchar <<= 1;
            if (mapchar == 0)
                mapchar = 1;
        }
        else
        {
            for (int i = 0; i < 1024; i++)
            {
                buffer[i] = mapchar;
            }
        }
        
        printf("sending bytes\n");
        // send bytes
        int s = write(spi, buffer, 1024);
        printf("done sending bytes\n");
        
        
        // wait for deassert
        while (fastDigitalRead(0) == LOW)
        {
            delayMicroseconds(10);
        }
        
        printf("done sending\n");
    }
    
    
    /*
    unsigned char buffer[256];
    
    for (int i = 0; i < 256; i++)
    {
        int c = i % 40;
        //int l = c % 26;
        buffer[i] = '0' + c;
    }
    
    
    unsigned char data[128];
    printf("hello.\n");
    wiringPiSetup();
    //pinMode(14, OUTPUT);
    //pullUpDnControl(14, PUD_OFF);
    
    int spi = wiringPiSPISetup(0, 2000000);
    printf("spi: %d\n", spi);
    pinMode(25, OUTPUT);
    pullUpDnControl(25, PUD_OFF);
    fastDigitalWrite(25, HIGH);
    
    
    printf("delay..\n");
    delayMicroseconds(1000000);
    fastDigitalWrite(25, LOW);
    printf("done.\n");
    
    data[0] = 0xFF;
    data[1] = 0xAA;
    //int r = write(spi, data, 2);
    //int r = read(spi, data, 2);
    int r = write(spi, buffer, 256);
    
    //int r = wiringPiSPIDataRW(0, data, 1);
    printf("data: %d %02X\n", data[0], data[0]);
    printf("data: %d %02X\n", data[1], data[1]);
    fastDigitalWrite(25, HIGH);
    */
     
    /*
    while(1)
    {
        delayMicroseconds(100000);
    }
    */
    
    //close(spi);
    
    
    //pinMode(14, INPUT);
    //pullUpDnControl(14, PUD_OFF);
    
    
    return 1;
}