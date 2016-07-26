#include <wiringPi.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
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
    digitalWrite(9, HIGH);
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
        digitalWrite(i, byte & 0x01);
        byte >>= 1;
    }
}

void wait_for_signal()
{
    bool found = false;
    while (!found)
    {
        int readval = digitalRead(8);
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
        int readval = digitalRead(8);
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
    digitalWrite(9, LOW);
}

void signal_notready()
{
    digitalWrite(9, HIGH);
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
    
    printf("cmd is %d\n", cmd);
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
    
    unsigned char ch = 'A';
    for (int i = 0; i < 100; i++)
    {
        receive_command();
        bytesReceived++;
    
        // send response
        set_port_output();
        for (int j = 0; j < 1000; j++)
        {
            send_byte_with_handshake(ch);
        }
        ch++;
        set_port_input();
    }
    
    gettimeofday(&endTime, NULL);
    double start = (double)startTime.tv_sec + ((double)startTime.tv_usec / 1000000.0);
    double end = (double)endTime.tv_sec + ((double)endTime.tv_usec / 1000000.0);
    double elapsed = end-start;
    
    printf("start %lf end %lf elapsed %lf bytes %d BPS %f\n", start, end, elapsed, bytesReceived, (float)bytesReceived / elapsed);
     
    return 1;
}