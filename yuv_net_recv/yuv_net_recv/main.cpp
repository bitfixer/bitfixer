//
//  main.cpp
//  yuv_net_recv
//
//  Created by Michael Hill on 9/19/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#include <iostream>
#include "NetPort.h"
#include "timer.hpp"
#include <string.h>

typedef struct {
    unsigned char frame;
    unsigned char chunk;
    unsigned char data[8000];
} frameChunk;

int main(int argc, const char * argv[]) {
    // insert code here...
    
    int chunksize = sizeof(frameChunk);
    int ipaddr[4] = {0, 0, 0, 0};
    int inport = 9999;
    int outport = 8888;
    
    frameChunk* chunk;
    
    unsigned char* buf = new unsigned char[chunksize];
    NetPort port(ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3],inport,outport);
    unsigned char yuvframe[96000];
    bool haveChunk[12];
    unsigned char curr_frame = 0;
    
    for (int i = 0; i < 12; i++)
    {
        haveChunk[i] = false;
    }
    
    while (1)
    {
        int ret = port.recv_sync(buf, chunksize);
        //printf("pport got %d bytes\n", ret);
        
        if (ret == chunksize)
        {
            chunk = (frameChunk*)buf;
            //printf("frame %d chunk %d\n", chunk->frame, chunk->chunk);
            
            if (chunk->frame != curr_frame) {
                // new frame
                // clear chunks
                for (int i = 0; i < 12; i++)
                {
                    haveChunk[i] = false;
                }
                curr_frame = chunk->frame;
            }
            
            // copy chunk data
            int index = chunk->chunk * 8000;
            memcpy(&yuvframe[index], chunk->data, 8000);
            
            // mark chunk as complete
            haveChunk[chunk->chunk] = true;
            
            // check for complete frame
            bool complete = true;
            for (int i = 0; i < 12; i++)
            {
                if (!haveChunk[i])
                {
                    complete = false;
                    break;
                }
            }
            
            if (complete)
            {
                //printf("complete frame! %d\n", chunk->frame);
                
                fprintf(stderr, "complete frame %d\n", chunk->frame);
                fwrite(yuvframe, 1, 96000, stdout);
                
                // clear chunks
                for (int i = 0; i < 12; i++)
                {
                    haveChunk[i] = false;
                }
            }
        }
    }
    
    return 0;
}
