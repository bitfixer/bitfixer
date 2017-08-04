#include <wiringPi.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>

class Timer
{
public:
    void start()
    {
        gettimeofday(&startTime, NULL);
    }
    void end()
    {
        gettimeofday(&endTime, NULL);
    }
    
    void report(const char *infostr)
    {
        double start = (double)startTime.tv_sec + ((double)startTime.tv_usec / 1000000.0);
        double end = (double)endTime.tv_sec + ((double)endTime.tv_usec / 1000000.0);
        double elapsed = end-start;
        printf("timer %s: %lf\n", infostr, elapsed);
    }
    
private:
    struct timeval startTime;
    struct timeval endTime;
};

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

bool wait_for_signal(int timeout_usec)
{
    bool found = false;
    int usec = 0;
    while (!found && (timeout_usec == -1 || usec < timeout_usec))
    {
        int readval = fastDigitalRead(8);
        if (readval == 0)
        {
            found = true;
        }
        else
        {
            delayMicroseconds(1);
            usec++;
        }
    }
    
    //printf("wait for signal timed out.\n");
    return found;
}

bool wait_for_signal_notready(int timeout_usec)
{
    bool found = false;
    int usec = 0;
    while (!found && (timeout_usec == -1 || usec < timeout_usec))
    {
        int readval = fastDigitalRead(8);
        if (readval == 1)
        {
            found = true;
        }
        else
        {
            delayMicroseconds(1);
            usec++;
        }
    }
    
    //printf("wait for notready timed out.\n");
    return found;
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
    delayMicroseconds(1);
    fastDigitalWrite(9, LOW);
    delayMicroseconds(1);
    fastDigitalWrite(9, HIGH);
}

unsigned char receive_byte_with_handshake()
{
    fastDigitalWrite(9, HIGH);
    wait_for_signal(-1);
    unsigned char byte = piReadByte();
    signal_ready();
    // c64 has received our signal
    wait_for_signal_notready(-1);
    signal_notready();
    return byte;
}

void send_byte_with_handshake(unsigned char byte)
{
    fastDigitalWrite(9, HIGH);
    while (!wait_for_signal(10000))
    {
        signal_notready();
    }
    piWriteByte(byte);
    signal_ready();
    while (!wait_for_signal_notready(10000))
    {
        signal_notready();
    }
    signal_notready();
}

unsigned char receive_command()
{
    unsigned char cmd;
    cmd = receive_byte_with_handshake();
    printf("cmd is %d\n", cmd);
    return cmd;
}

// test - watch for input
int main(int argc, char **argv)
{
    //init();
    
    unsigned char buffer[256];
    
    
    for (int i = 0; i < 256; i++)
    {
        int c = i % 40;
        int letter = c % 26;
        buffer[i] = 'A' + letter;
    }
    
    
    
    wiringPiSetup();
    set_port_output();
    
    pinMode(8, INPUT);
    pullUpDnControl(8, PUD_OFF);
    
    pinMode(9, INPUT);
    pullUpDnControl(9, PUD_OFF);
    
    pinMode(10, OUTPUT);
    pullUpDnControl(10, PUD_OFF);
    fastDigitalWrite(10, HIGH);
    
    while (fastDigitalRead(8) == HIGH) {}
    fastDigitalWrite(10, LOW);
    for (int i = 0; i < 256; i++)
    {
        piWriteByte(buffer[i]);
        fastDigitalWrite(10, HIGH);
        while (fastDigitalRead(9) == LOW) {}
        while (fastDigitalRead(9) == HIGH) {}
        fastDigitalWrite(10, LOW);
    }
    
    /*
    for (int i = 0; i < 256; i += 2)
    {
        int col = i % 40;
        if (col > 25) col -= 26;
        piWriteByte('A'+col);
        // signal byte is ready
        fastDigitalWrite(8, LOW);
        
        // clock delay
        delayMicroseconds(150);
        
        col = (i+1) % 40;
        if (col > 25) col -= 26;
        piWriteByte('A'+col);
        fastDigitalWrite(8, HIGH);
        
        delayMicroseconds(150);
    }
    */
    
    printf("done.\n");
    return 1;
}