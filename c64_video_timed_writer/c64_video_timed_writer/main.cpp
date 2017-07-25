//
//  main.cpp
//  c64_video_timed_writer
//
//  Created by Michael Hill on 7/25/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include "timer.hpp"
#include "NetPort.h"

#define C64_FRAME_SIZE 9216

typedef struct
{
    float pts;
    unsigned char data[C64_FRAME_SIZE];
} C64_Frame;

int main(int argc, const char * argv[]) {
    FILE* fp = stdin;
    FILE* fp_out = stdout;
    C64_Frame frame;
    Tools::Timer timer;
    
    NetPort port(127,0,0,1,5555,8888);
    
    int frameSize = sizeof(frame);
    float start_pts = -1;
    while (fread(&frame, 1, frameSize, fp) == frameSize)
    {
        //fprintf(stderr, "pts: %f\n", frame.pts);
        if (start_pts < 0.0)
        {
            start_pts = frame.pts;
            timer.start();
        }
        
        float rel_pts = frame.pts - start_pts;
        while (timer.getTime() < rel_pts)
        {
            float sleepTimeUs = (rel_pts - timer.getTime()) * 1000000.0;
            if (sleepTimeUs > 1.0)
            {
                //fprintf(stderr, "sleep for %f us\n", sleepTimeUs);
                usleep(sleepTimeUs);
            }
        }
        
        fprintf(stderr, "sending frame at time %lf relpts %f\n", timer.getTime(), rel_pts);
        
        // write data to output
        //fwrite(frame.data, 1, C64_FRAME_SIZE, fp_out);
        int ret = port.send(frame.data, C64_FRAME_SIZE);
    }
    
    return 0;
}
