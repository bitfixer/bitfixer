#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include "timer.hpp"
//#include "rpiSoftSpi.h"
#include "rpiThreeWireSPI.h"

int main(int argc, char **argv)
{
    const char* fname = argv[1];
    printf("load %s\n", fname);

    unsigned char buffer[512];
    Tools::Timer t;
    int value = 0;
    rpiThreeWireSPI spi(1, // clock
                        0, // chip select
                        3, // miso
                        2 // mosi
                        );

    wiringPiSetup();
    spi.init();

    int bytesToSend = 8;
    memset(buffer, 0, 512);
    FILE* fp = fopen(fname, "rb");
    fread(buffer, 1, bytesToSend, fp);
    fclose(fp);

    while(1)
    {
        // get filename
        spi.transfer(buffer, 8);
        buffer[8] = 0;
        printf("filename: %s\n", buffer);

        FILE* prgfp = fopen("msh.prg", "rb");
        fseek(prgfp, 0, SEEK_END);
        uint16_t size = (uint16_t)ftell(prgfp);
        fseek(prgfp, 0, SEEK_SET);

        // send the file length
        //unsigned char* sizeBytes = (unsigned char*)&size;
        unsigned char sizeBytes[2];
        sizeBytes[0] = (size & 0xFF00) >> 8;
        sizeBytes[1] = size & 0x00FF;

        spi.transfer(sizeBytes, 2);

        int bytes_read = 0;
        do {
            bytes_read = fread(buffer, 1, 512, prgfp);
            spi.transfer(buffer, 512);
            printf("sent %d\n", bytes_read);
        } while(bytes_read == 512);

        fclose(prgfp);
        printf("done.\n");
    }


    while(1)
    {
        for (int i = 0; i < 512; i++)
        {
            buffer[i] = i+5;
        }

        //t.start();
        int bytesTransferred = spi.transfer(buffer, 100);
        //double elapsed = t.getTime();
        //double rate = (double)bytesToSend / elapsed;

        bool dataGood = true;
        for (int i = 0; i < bytesTransferred; i++)
        {
            printf("buffer %d: %X\n", i, buffer[i]);
            if (i % 256 != buffer[i])
            {
                dataGood = false;
            }
        }
    }
    //printf("done. took %lf seconds, Bps %lf. DataGood: %d\n", elapsed, rate, dataGood);



    return 1;
}
