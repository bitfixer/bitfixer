//
//  main.cpp
//  c64_video_timed_writer
//
//  Created by Michael Hill on 7/25/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timer.hpp"
#include "NetPort.h"

#define PET_FRAME_SIZE 2000
#define PET_40_FRAME_SIZE 1000
#define C64_FRAME_SIZE 9216

class Frame
{
public:
    Frame(int dataSize)
    : _dataSize(dataSize)
    {
        _data = new unsigned char[_dataSize];
    }
    
    ~Frame()
    {
        if (_data)
        {
            delete[] _data;
        }
    }
    
    void setPTS(float pts)
    {
        _pts = pts;
    }
    
    float pts()
    {
        return _pts;
    }
    
    int size()
    {
        return sizeof(_pts) + _dataSize;
    }
    
    unsigned char* data()
    {
        return _data;
    }
    
    int dataSize()
    {
        return _dataSize;
    }
    
    bool read(FILE* fp)
    {
        if (fread(&_pts, sizeof(float), 1, fp) != 1)
        {
            return false;
        }
        
        if (fread(_data, 1, _dataSize, fp) != _dataSize)
        {
            return false;
        }
        
        return true;
    }
    
private:
    int _dataSize;
    unsigned char* _data;
    float _pts;
};

int main(int argc, char * const argv[]) {
    // initialize
    int c;
    int frame_size = C64_FRAME_SIZE;
    int output_port = 99999;
    FILE* fp = stdin;
    int ip[4] = {127, 0, 0, 1};
    
    while ((c = getopt(argc, argv, "t:p:i:a:")) != -1)
    {
        if (c == 't')
        {
            if (strcmp(optarg, "c64") == 0)
            {
                frame_size = C64_FRAME_SIZE;
            }
            else if (strcmp(optarg, "pet") == 0)
            {
                frame_size = PET_FRAME_SIZE;
            }
            else if (strcmp(optarg, "pet40") == 0)
            {
                frame_size = PET_40_FRAME_SIZE;
            }
        }
        else if (c == 'p')
        {
            output_port = atoi(optarg);
        }
        else if (c == 'i')
        {
            fp = fopen(optarg, "rb");
        }
        else if (c == 'a') // ip address
        {
            sscanf(optarg, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
        }
    }
    
    Frame frame(frame_size);
    Tools::Timer timer;
    NetPort port(ip[0],ip[1],ip[2],ip[3],5555,output_port);
    NetPort timeListenerPort(127,0,0,1,5556,output_port);
    
    unsigned char temp[2000];
    
    float start_pts = -1;
    double external_offset = 0.0;
    while (frame.read(fp))
    {
        fprintf(stderr, "pts: %f\n", frame.pts());
        if (start_pts < 0.0)
        {
            start_pts = frame.pts();
            timer.start();
        }
        
        float rel_pts = frame.pts() - start_pts;
        
        // get current time
        double currentTime = timer.getTime() + external_offset;
        fprintf(stderr, "current time: %lf timer %lf offset %lf\n", currentTime, timer.getTime(), external_offset);
        while (currentTime < rel_pts)
        {
            float sleepTimeUs = (rel_pts - currentTime) * 1000000.0;
            
            // check for timer update
            uint32_t nTimeUs = 0;
            int ret = timeListenerPort.recv((unsigned char*)&nTimeUs, sizeof(uint32_t));
            if (ret == sizeof(uint32_t))
            {
                uint32_t timeUs = ntohl(nTimeUs);
                printf("got time update: %d at time %lf\n", timeUs, timer.getTime());
                
                double ext_time = (double)timeUs / 1000000.0;
                external_offset = ext_time - timer.getTime();
            }
            
            if (sleepTimeUs > 1.0)
            {
                usleep(sleepTimeUs);
            }
            
            currentTime = timer.getTime() + external_offset;
        }
        
        fprintf(stderr, "sending frame at time %lf relpts %f error %f\n", currentTime, rel_pts, currentTime-rel_pts);
        
        // write data to output
        //fwrite(frame.data, 1, C64_FRAME_SIZE, fp_out);
        //memcpy(temp, frame.data(), frame.dataSize());
        int ret = port.send(frame.data(), frame.dataSize());
        //int ret = port.send(temp, 2000);
        //printf("send returned %d op %d %d %d %d %d\n", ret, ip[0], ip[1], ip[2], ip[3], output_port);
    }
    
    if (fp != stdin)
    {
        fclose(fp);
    }
    
    return 0;
}
