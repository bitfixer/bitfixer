#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include "timer.hpp"
#include "rpiSpiData.h"
#include "c64drive.h"
 
typedef enum
{
    Idle,
    GettingFilename,
    Saving,
    Loading
} driveState;

typedef struct
{
    int resetPin;
    int spiReqPin;
    int spiInterface;
    rpiSpiData* spi_data;
} spiInfo;

// test - watch for input
int main(int argc, char **argv)
{

    spiInfo spiInfo[2];
    
    // SPI 0
    /*
    int resetPin = 3; // wiringPi number
    // BCM GPIO 22
    int spiReqPin = 0;
    int spiInterface = 0;
    */
    
    spiInfo[0].resetPin = 3;
    spiInfo[0].spiReqPin = 0;
    spiInfo[0].spiInterface = 0;
    
    spiInfo[1].resetPin = 7;
    spiInfo[1].spiReqPin = 4;
    spiInfo[1].spiInterface = 1;
    
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
    
    
    
     
    // SPI 1
    //int resetPin = 7; // wiringPi number
    // BCM GPIO 4
    //int spiReqPin = 4;
    //int spiInterface = 1;
    
    /*
    wiringPiSetup();
    pinMode(spiReqPin, INPUT);
    pullUpDnControl(spiReqPin, PUD_UP);

    pinMode(resetPin, OUTPUT);
    pullUpDnControl(resetPin, PUD_OFF);
    digitalWrite(resetPin, LOW); // reset MCU

    //int spi = wiringPiSPISetup(0, 2000000);
    int spi = wiringPiSPISetup(spiInterface, 2000000);
    rpiSpiData spi_data(spiReqPin, spi);
    */
     
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
                spiInfo[i].spi_data->send(pkt, 1);
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
