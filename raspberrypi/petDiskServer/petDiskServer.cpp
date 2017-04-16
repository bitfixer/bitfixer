#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include "timer.hpp"
#include "rpiSoftSpi.h"

int main(int argc, char **argv)
{
    const char* fname = argv[1];
    printf("load %s\n", fname);
    
    unsigned char buffer[256];
    Tools::Timer t;
    int value = 0;
    rpiSoftSPI spi(1, // clock
                   0, // chip select
                   3, // miso
                   2, // mosi
                   true
                   );
    
    wiringPiSetup();
    spi.init();
    
    int bytesToSend = 200;
    memset(buffer, 0, 256);
    FILE* fp = fopen(fname, "rb");
    fread(buffer, 1, bytesToSend, fp);
    fclose(fp);
    
    while (!spi.isSelected())
    {
        delayMicroseconds(1000);
    }
    
    t.start();
    spi.send(buffer, strlen((const char*)buffer));
    
    double elapsed = t.getTime();
    double rate = (double)bytesToSend / elapsed;
    printf("done. took %lf seconds, Bps %lf\n", elapsed, rate);
    
    /*
    buffer[0] = 0xA0;
    while(1)
    {
        if (spi.isSelected())
        {
            spi.send(buffer, 1);
            buffer[0]++;
        }
        else
        {
            delayMicroseconds(1000);
        }
    }
    
    printf("checking for commands..\n");
    */
    
    return 1;
}
