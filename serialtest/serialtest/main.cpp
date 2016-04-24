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
#include "SidController.h"
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

typedef struct
{
    double time;
    double sendTime;
    int unique_id;
    unsigned char buffer[256];
    int size;
} TimingInfo;


template <class T>
class CircularBuffer
{
public:
    CircularBuffer(int maxSize) :
    maxSize(maxSize)
    {
        buffer = (T *)malloc(sizeof(T) * maxSize);
    }
    
    int size()
    {
        return count;
    }
    
    T *top()
    {
        return &buffer[readIndex];
    }
    
    void add(T *obj)
    {
        T *writePtr = &buffer[writeIndex];
        memcpy(writePtr, obj, sizeof(T));
        writeIndex++;
        if (writeIndex >= maxSize)
        {
            writeIndex = 0;
        }
        count++;
    }
    
    void pop()
    {
        readIndex++;
        if (readIndex >= maxSize)
        {
            readIndex = 0;
        }
        count--;
    }
    
private:
    T *buffer;
    int maxSize = 0;
    int writeIndex = 0;
    int readIndex = 0;
    int count = 0;
};


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
    
    // make serial port
    SerialPort *port = new SerialPort(argv[1], (int)bauds);
    sleep(2);
    //SidController *controller = new SidController(port);
    
    net::Socket socket;
    int sock_port = 29999;
    if (!socket.Open(sock_port))
    {
        printf("boo\n");
    }
    
    TimingInfo timingInfo;
    CircularBuffer<TimingInfo> timedPackets(128);
    Timer timer;
    timer.start();
    int port_id = -1;
    double timeRefLocal = -1.0;
    double timeRefRemote = -1.0;
    
    while (timer.getTime() < 120.0)
    {
        net::Address sender;
        unsigned char buffer[256];
        int bytes_read = socket.Receive( sender, buffer, sizeof( buffer ) );
        if (bytes_read )
        {
        //printf( "received packet from %d.%d.%d.%d:%d (%d bytes)\n",
        //	sender.GetA(), sender.GetB(), sender.GetC(), sender.GetD(),
        //	sender.GetPort(), bytes_read );
            
            // relay the bytes
            //port->send(buffer, bytes_read);
            memcpy(&timingInfo, buffer, bytes_read);
            timingInfo.size = bytes_read - 2*sizeof(double);
            //printf("time %lf sendTime %lf\n", timingInfo.time, timingInfo.sendTime);
            
            if (port_id != timingInfo.unique_id)
            {
                // get time reference
                timeRefLocal = timer.getTime();
                timeRefRemote = timingInfo.time;
            }
            
            timedPackets.add(&timingInfo);
        }
        else
        {
            // check for timed packet to send
            if (timedPackets.size() > 0)
            {
                TimingInfo *t = timedPackets.top();
                double sendTime = t->sendTime;
                //double now = timer.getTime() + (timeRefRemote-timeRefLocal);
                double localTime = timer.getTime();
                double now = (localTime-timeRefLocal) + timeRefRemote;
                
                if (now > sendTime || sendTime-now < 0.0001)
                {
                    //printf("sending now %f st %f\n", now, sendTime);
                    int result = port->send(t->buffer, t->size);
                    if (result > 0)
                    {
                        float time_per_byte = 10.0 / (float)bauds;
                        float time_to_send = time_per_byte * result;
                        int usec = (int)ceil(time_to_send * 1000000.0);
                        usleep(usec);
                    }
                    
                        // remove packet
                    timedPackets.pop();
                }
                else
                {
                    usleep(100);
                }
            }
            else
            {
                usleep(100);
            }
        }
    }
    
    /*
    float freq = 440.0;
    controller->init();
    controller->setVolume(1.0);
    
    for (int i = 0; i < 3; i++)
    {
        controller->setFrequency(i, freq * powf(1.2, (float)i));
        controller->setSustain(i, 0.85);
        controller->setRelease(i, 0.0);
        controller->setWaveform(i, Sid::waveForm::SAWTOOTH);
    }
    
    
    //controller->setFrequency(2, 220.0);
    //controller->setSustain(2, 0.85);
    //controller->setRelease(2, 0.1);
    //controller->setWaveform(2, Sid::waveForm::NOISE);
     
    Timer t;
    t.start();
    t.end();
    
    double notetime = 60.0 / (150.0*4.0);
    int notenum = 0;
    while (notenum < 40)
    {
        // play note
        controller->noteOn(0);
        controller->noteOn(1);
        controller->noteOn(2);
        double halfNote = (notetime / 2);
        int usec_halfnote = (int)(halfNote * 1000000.0);
        usleep(usec_halfnote);
        controller->noteOff(0);
        controller->noteOff(1);
        controller->noteOff(2);
        
        t.end();
        double time = t.getTime();
        notenum++;
        double nextTime = (double)notenum * notetime;
        double waittime = nextTime - time;
        int usec = (int)floor(waittime * 1000000.0);
        
        usleep(usec);
    }
    */
    
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
     
    //unsigned char tt = 255;
    //port->send(&tt, 1);
    
    sleep(2);
    delete port;
    sleep(2);
    return 0;
}
