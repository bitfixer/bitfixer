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
    char temp[256];
    memset(buffer, 0, 1024);
    
    wiringPiSetup();
    pinMode(0, INPUT);
    pullUpDnControl(0, PUD_OFF);
    
    int spi = wiringPiSPISetup(0, 2000000);
    
    printf("checking for commands..\n");
    
    int frame = 0;
    FILE *fp = NULL;
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
            if (fp)
            {
                fclose(fp);
                fp = NULL;
            }
            sprintf(temp, "pframe_%04d.c64", frame);
            printf("reading %s\n", temp);
            fp = fopen(temp, "rb");
            
            frame = (frame+1) % 20;
            
            // read first block
            fread(buffer, 1, 1000, fp);
            
            /*
            for (int i = 0; i < 1024; i++)
            {
                //int c = i % 40;
                //int l = (c + cmd) % 26;
                //buffer[i] = 'A' + l;
                buffer[i] = 0x01;
            }
            */
        }
        else
        {
            fread(buffer, 1, 1024, fp);
            
            /*
            for (int i = 0; i < 1024; i++)
            {
                buffer[i] = mapchar;
            }
            */
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
    
    return 1;
}