#include <wiringPi.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include "commands.h"

void set_port_input()
{
    for (int i = 0; i < 8; i++)
    {
        pinMode(i, INPUT);
        pullUpDnControl(i, PUD_OFF);
    }
}

void set_port_output()
{
    for (int i = 0; i < 8; i++)
    {
        pinMode(i, OUTPUT);
        pullUpDnControl(i, PUD_OFF);
    }
}

void init()
{
    wiringPiSetup();
    // set all pins input
    set_port_input();
    
    pinMode(8, INPUT);
    pullUpDnControl(8, PUD_DOWN);
    
    pinMode(9, OUTPUT);
    fastDigitalWrite(9, HIGH);
}

unsigned char piReadByte()
{
    unsigned char byte = 0;
    for (int i = 7; i >= 0; i--)
    {
        byte <<= 1;
        byte += fastDigitalRead(i);
    }
    return byte;
}

void piWriteByte(unsigned char byte)
{
    unsigned char bit = 0;
    for (int i = 0; i < 8; i++)
    {
        fastDigitalWrite(i, byte & 0x01);
        byte >>= 1;
    }
}

void wait_for_signal()
{
    bool found = false;
    while (!found)
    {
        int readval = fastDigitalRead(8);
        if (readval == 0)
        {
            found = true;
        }
    }
}

void wait_for_signal_notready()
{
    bool found = false;
    while (!found)
    {
        int readval = fastDigitalRead(8);
        if (readval == 1)
        {
            found = true;
        }
    }
}

void signal_ready()
{
    // lower the handshake line
    // c64 is looking for negative edge
    fastDigitalWrite(9, LOW);
}

void signal_notready()
{
    fastDigitalWrite(9, HIGH);
}

unsigned char receive_byte_with_handshake()
{
    wait_for_signal();
    unsigned char byte = piReadByte();
    signal_ready();
    // c64 has received our signal
    wait_for_signal_notready();
    signal_notready();
    return byte;
}

void send_byte_with_handshake(unsigned char byte)
{
    wait_for_signal();
    // put byte on bus
    piWriteByte(byte);
    signal_ready();
    wait_for_signal_notready();
    signal_notready();
}

void receive_command()
{
    unsigned char cmd;
    cmd = receive_byte_with_handshake();
    //printf("cmd is %d\n", cmd);
}

void create_test_bitmap(unsigned char *dest, int width, int height)
{
    // first clear the bitmap
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            dest[h*width + w] = 0;
        }
    }
    
    float xcenter = (float)width/(float)2.0;
    float ycenter = (float)height/(float)2.0;
    float radius = 60.0;
    float linewidth = 5.0;
    
    // draw a circle
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            float xpos = (float)w + 0.5;
            float ypos = (float)h + 0.5;
            
            float xdist = xpos-xcenter;
            float ydist = ypos-ycenter;
            
            float thisrad = sqrt(xdist*xdist + ydist*ydist);
            if (thisrad >= radius - (linewidth/2.0) &&
                thisrad <= radius + (linewidth/2.0))
            {
                dest[h*width + w] = 1;
            }
        }
    }
}

// input - pixel-addressed bitmap
// output - c64 bitmap bytes
void create_c64_bitmap(unsigned char *dest, unsigned char *src, int width, int height)
{
    int c64bytes = (width/8) * (height/8);
    // clear
    memset(dest, 0, c64bytes);
    
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            if (dest[h*width + w] == 1)
            {
                int row = h/8;
                int c = w/8;
                int line = h & 7;
                int bit = 7 - (w & 7);
                int byte = row*320 + c*8 + line;
                
                unsigned char b = dest[byte];
                unsigned char mask = 1 << bit;
                b = b | mask;
                dest[byte] = b;
            }
        }
    }
    
    
    
}

// test - watch for input
int main(void)
{
    struct timeval startTime;
    struct timeval endTime;
    bool started = false;
    init();
    
    bool done = false;
    int bytesReceived = 0;
    unsigned char bitmap[320*200];
    unsigned char c64_bitmap[8000];
    
    create_test_bitmap(bitmap, 320, 200);
    create_c64_bitmap(c64_bitmap, bitmap, 320, 200);
    printf("ready.\n");
    
    /*
    while (!done)
    {
        unsigned char byte = receive_byte_with_handshake();
        if (!started)
        {
            started = true;
            gettimeofday(&startTime, NULL);
        }
        printf("got byte: %d\n", byte);
        bytesReceived++;
        
        if (bytesReceived % 100 == 0)
        {
            printf("br %d\n", bytesReceived);
        }
        
        if (byte == 0xff)
            done = true;
    }
    
    // output
    set_port_output();
    for (int i = 0; i < 255; i++)
    {
        send_byte_with_handshake(100+i);
    }
    set_port_input();
    */
    
    /*
    unsigned char ch = 'A';
    for (int i = 0; i < 10; i++)
    {
        receive_command();
        if (!started)
        {
            started = true;
            gettimeofday(&startTime, NULL);
        }
    
        // send response
        set_port_output();
        for (int j = 0; j < 1000; j++)
        {
            send_byte_with_handshake(ch);
        }
        ch++;
        
        bytesReceived += 1001;
        
        set_port_input();
    }
    */
    
    // send one bitmap frame
    receive_command();
    gettimeofday(&startTime, NULL);
    
    set_port_output();
    for (int j = 0; j < 8000; j++)
    {
        send_byte_with_handshake(c64_bitmap[j]);
    }
    bytesReceived += 8000;
    set_port_input();
     
    gettimeofday(&endTime, NULL);
    double start = (double)startTime.tv_sec + ((double)startTime.tv_usec / 1000000.0);
    double end = (double)endTime.tv_sec + ((double)endTime.tv_usec / 1000000.0);
    double elapsed = end-start;
    
    printf("start %lf end %lf elapsed %lf bytes %d BPS %f\n", start, end, elapsed, bytesReceived, (float)bytesReceived / elapsed);
     
    return 1;
}