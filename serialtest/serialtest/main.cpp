//
//  main.cpp
//  serialtest
//
//  Created by Michael Hill on 3/27/16.
//  Copyright (c) 2016 Michael Hill. All rights reserved.
//

#include "SerialPort.hpp"
#include <math.h>
#include <iostream>
#include "sid.h"
#include "timer.hpp"
#include <unistd.h>
#include "Net.h"
    
using namespace std;


// SID Frequency table
// Fn = Fout/.06097
class SIDTable
{
public:
    SIDTable() {}
    
    static void bytesForFrequency(float freq, unsigned char &low, unsigned char &high)
    {
        float fn = freq / 0.06097;
        int16_t fn_int = (int16_t)floor(fn);
        low = fn_int & 0x00FF;
        high = (fn_int & 0xFF00) >> 8;
    }
};
// things to add:
// set frequency
// set filter
// set waveform
// set adsr




int main(int argc, const char * argv[])
{
    if (argc < 2 || argc > 3)
    {
        cerr << "usage: " << argv[0] << " device [bauds]" << endl;
        return 1;
    }
    
    string device = argv[1];
    unsigned long bauds = 9600;
    if (argc == 3)
    {
        char* result;
        bauds = strtoul(argv[2], &result, 10);
        if (*result != '\0')
        {
            cerr << "usage: " << argv[0] << " device [bauds]" << endl;
            return 1;
        }
    }
    
    float freq = 50.0;
    float fmax = 2000.0;
    float steps = 100.0;
    // fst * r^n = fmax
    float rn = fmax / freq;
    float r = powf(rn, 1.0/steps);
    printf("r = %f\n", r);
    
    int st = (int)steps;
    
    for (int i = 0; i < 3; i++)
    {
        int offset = offsetof(struct __sid, v[i]);
        printf("v[%d] = %d\n", i, offset);
    }
    
    printf("size is %d\n", sizeof(struct __sid));
    
    // make serial port
    SerialPort *port = new SerialPort(argv[1], (int)bauds);
    SidController *controller = new SidController(port);
    
    net::Socket socket;
    int sock_port = 30000;
    if (!socket.Open(sock_port))
    {
        printf("boo\n");
    }
    
    sleep(2);
    
    
    controller->init();
    controller->setVolume(1.0);
    
    for (int i = 0; i < 2; i++)
    {
        controller->setFrequency(i, freq*(i+1));
        controller->setSustain(i, 0.85);
        controller->setRelease(i, 0.1);
        controller->setWaveform(i, Sid::waveForm::SAWTOOTH);
    }
    
    controller->setFrequency(2, 220.0);
    controller->setSustain(2, 0.85);
    controller->setRelease(2, 0.1);
    controller->setWaveform(2, Sid::waveForm::NOISE);
    
    
    // play note
    controller->noteOn(0);
    
    Timer t;
    t.start();
    t.end();
    unsigned char buffer[256];
    
    while (t.getTime() < 30.0)
    {
        net::Address sender;
        int bytes_read = socket.Receive(sender, buffer, sizeof(buffer));
        if (bytes_read)
        {
            float *data = (float *)buffer;
            printf("freq %f\n", *data);
            
            float harmonic = *data * 2.0;
            controller->setFrequency(0, harmonic);
        }
        
        t.end();
    }
    
    controller->noteOff(0);
    
    /*
    for (int i = 0; i < 2; i++)
    {
        controller->noteOn(i);
    }
    
    for (int i = 0; i < st; i++)
    {
        freq *= r;
        printf("freq is %f\n", freq);
        controller->setFrequency(0, freq);
        controller->setFrequency(1, freq*1.5);
        usleep(50000);
    }
    
    for (int i = 0; i < 3; i++)
    {
        controller->noteOff(i);
    }
    */
    
    /*
    t.start();
    double interval = 1.0 / 4.0;
    for (int i = 0; i < 20; i++)
    {
        t.end();
        double noteStartTime = t.getTime();
        controller->noteOn(2);
        usleep(100000);
        controller->noteOff(2);
        
        // measure time
        t.end();
        double currTime = t.getTime();
        
        // calculate next time
        double nextTime = (double)(i+1) * interval;
        double waitTime = nextTime - currTime;
        long useconds = (long)round(waitTime * 1000000.0);
        
        printf("st %lf en %lf nx %lf us %ld", noteStartTime, currTime, nextTime, useconds);
        usleep(useconds);
        
        //usleep(50000);
    }
    
    for (int i = 0; i < 3; i++)
    {
        controller->noteOff(i);
    }
    */
    
    /*
    for (int i = 0; i < 40; i++)
    {
        controller->noteOn(0);
        //usleep(10000);
        controller->noteOff(0);
        usleep(100000);
    }
    */
    
    /*
    controller->noteOn(0);
    float curr = 220.0;
    for (int i = 0; i < 400; i++)
    {
        if (curr == 220.0)
            curr = 440.0;
        else
            curr = 220.0;
        
        controller->setFrequency(0, curr);
        
        usleep(15000);
    }
    controller->noteOff(0);
    */
     
    unsigned char tt = 255;
    port->send(&tt, 1);
    
    sleep(2);
    //close(fd);
    delete port;
    sleep(2);
    return 0;
    
    /*
    char buffer[80];
    sprintf(buffer, "QQQQQQQQQQ");
    port->send((unsigned char *)buffer, (int)strlen(buffer));
    // TEMP
    sleep(2);
    delete port;
    sleep(2);
    return 0;
    */
     
    /*
    for (unsigned char i = 0; i < 254; i++)
    {
        port->send(&i, 1);
        usleep(100000);
    }
    */
    
    /*
     30 poke 54272+24, 15
     40 poke 54272+1, 100
     50 poke 54272+6, 15*16+9
     60 poke 54272+8, 70
     70 poke 54272+13, 10*16+9
    */
    
    /*
    send_control_packet(port, 24, 15);
    send_control_packet(port, 1, 100);
    send_control_packet(port, 6, 15*16+9);
    send_control_packet(port, 8, 70);
    send_control_packet(port, 13, 10*16+9);
    
    float freqs[8];
    freqs[0] = 261.63;
    freqs[1] = 293.66;
    freqs[2] = 329.63;
    freqs[3] = 349.23;
    freqs[4] = 392.00;
    freqs[5] = 440.00;
    freqs[6] = 493.88;
    freqs[7] = 523.25;
    
    for (int i = 0; i < 8; i++)
    {
        unsigned char addr = 0;
        unsigned char low, high;
        unsigned char cmd;
        
        SIDTable::bytesForFrequency(freqs[i], low, high);
        printf("freq %f, bytes %d %d\n", freqs[i], low, high);
        
        send_control_packet(port, addr, low);
        addr = 1;
        send_control_packet(port, addr, high);
        
        addr = 4;
        cmd = 16+1;
        send_control_packet(port, addr, cmd);
        
        cmd = 16;
        send_control_packet(port, addr, cmd);
        
        usleep(200000);
    }
    
    unsigned char ttt = 'Q';
    port->send(&ttt, 1);
*/

    /*
    for (int i = 0; i < 10; i++)
    {
        unsigned char addr = 4;
        port->send(&addr, 1);
        unsigned char cmd = 16+1;
        port->send(&cmd, 1);
        port->send(&addr, 1);
        cmd = 16;
        port->send(&cmd, 1);
        
        cmd = 16+1;
        addr = 11;
        port->send(&addr, 1);
        port->send(&cmd, 1);
        port->send(&addr, 1);
        cmd = 16;
        port->send(&cmd, 1);
        
        usleep(250000);
        
        cmd = 16+1;
        addr = 11;
        port->send(&addr, 1);
        port->send(&cmd, 1);
        port->send(&addr, 1);
        cmd = 16;
        port->send(&cmd, 1);
        
        usleep(250000);
    }
    */
     
    
    /*
    int fd = open(argv[1], O_RDWR | O_NDELAY | O_NOCTTY);
    if (fd == -1)
    {
        perror((string("can't open ") + argv[1]).c_str());
        exit(errno);
    }
    
    struct termios config;
    if (tcgetattr(fd, &config) < 0)
    {
        perror("can't get serial attributes");
        exit(errno);
    }
    
    if (cfsetispeed(&config, bauds) < 0 || cfsetospeed(&config, bauds) < 0)
    {
        perror("can't set baud rate");
        exit(errno);
    }
    
    config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
    config.c_oflag = 0;
    config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
    config.c_cflag &= ~(CSIZE | PARENB);
    config.c_cflag |= CS8 | CLOCAL;
    config.c_cc[VMIN]  = 1;
    config.c_cc[VTIME] = 0;
    
    if (tcsetattr(fd, TCSAFLUSH, &config) < 0)
    {
        perror("can't set serial attributes");
        exit(errno);
    }
    
    char buffer[80];
    */
    
    /*
    while (true)
    {
        size_t n = read(fd, buffer, sizeof buffer);
        write(STDOUT_FILENO, buffer, n);
        
        if (n > 0)
        {
            char t = 'A';
            write(fd, &t, 1);
        }
    }
    */
    
    /*
    sprintf(buffer, "YOU ARE COOL");
    sleep(1);
    write(fd, buffer, strlen(buffer));
    */
    
    // not sure why, but system locks up if these sleeps are not here
    sleep(2);
    //close(fd);
    delete port;
    sleep(2);
    return 0;
}
