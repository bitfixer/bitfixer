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

/*
typedef struct
{
    unsigned char state;
    unsigned char ss;
    bool isAtn;
    unsigned char size;
    unsigned char buffer[128];
    unsigned char atn_size;
    unsigned char atn_buffer[8];
} dataPacket;
*/
 
typedef enum
{
    Idle,
    GettingFilename,
    Saving,
    Loading
} driveState;

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
    driveState state = Idle;
    
    char fname[256];
    int fname_len = 0;
    
    FILE* fp_load = NULL;
    unsigned char program[1024];
    int program_size = 0;
    int program_bytes_sent = 0;

    while (1)
    {
        int recv_size = spi_data.receive(pkt);
        if (recv_size > 0)
        {
            dataPacket* dpkt = (dataPacket*)pkt;
            printf("recv %d bytes data bytes %d atn bytes %d\n", recv_size, dpkt->data_size, dpkt->atn_size);
            
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
                            /*
                            // close open file
                            fname[fname_len] = 0;
                            printf("saving file %s data size %d\n", fname, program_size);
                            
                            FILE* fp = fopen(fname, "wb");
                            fwrite(program, 1, program_size, fp);
                            fclose(fp);
                            
                            fname_len = 0;
                            program_size = 0;
                            */
                            
                            printf("closing %s\n", fname);
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
                        if (b == 0x3F)
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
                    /*
                    if (atn_byte < dpkt->atn_size)
                    {
                        printf("SAVING ATN %d: %c %d %X\n", atn_byte, dpkt->atn_buffer[atn_byte], dpkt->atn_buffer[atn_byte], dpkt->atn_buffer[atn_byte]);
                        unsigned char b = dpkt->atn_buffer[atn_byte++];
                        if (b == 0x3F)
                        {
                            // reading a filename
                            state = Idle;
                        }
                    }
                    else if (data_byte < dpkt->data_size)
                    {
                        printf("SAVING %d: %c %d %X\n", data_byte, dpkt->data_buffer[data_byte], dpkt->data_buffer[data_byte], dpkt->data_buffer[data_byte]);
                        char b = dpkt->data_buffer[data_byte++];
                        program[program_size++] = b;
                    }
                    */
                    printf("saving\n");
                    if (dpkt->data_size > 0)
                    {
                        if (!fp_load)
                        {
                            fname[fname_len] = 0;
                            printf("opening %s\n", fname);
                            fp_load = fopen(fname, "wb");
                        }
                        
                        printf("saving %s: %d bytes\n", fname, dpkt->data_size);
                        fwrite(dpkt->data_buffer, 1, dpkt->data_size, fp_load);
                        data_byte += dpkt->data_size;
                    }
                    else if (atn_byte < dpkt->atn_size)
                    {
                        printf("SAVING ATN %d: %c %d %X\n", atn_byte, dpkt->atn_buffer[atn_byte], dpkt->atn_buffer[atn_byte], dpkt->atn_buffer[atn_byte]);
                        unsigned char b = dpkt->atn_buffer[atn_byte++];
                        if (b == 0x3F)
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
            printf("response\n");
            spi_data.send(pkt, sizeof(dataPacket));
        }
    }
    
    /*
    while (!done)
    {
        int recv_size = spi_data.receive(pkt);
        if (recv_size > 0)
        {
            dataPacket* dpkt = (dataPacket*)pkt;
            
            for (int i = 0; i < dpkt->atn_size; i++)
            {
                printf("ATN %d: %c %d %X\n", i, dpkt->atn_buffer[i], dpkt->atn_buffer[i], dpkt->atn_buffer[i]);
            }
            
            for (int i = 0; i < dpkt->size; i++)
            {
                printf("%d: %c %d %X\n", i, dpkt->data_buffer[i], dpkt->data_buffer[i], dpkt->data_buffer[i]);
            }
            
            // send state response
            spi_data.send(pkt, 1);
        }
    }
    */

    return 1;
}
