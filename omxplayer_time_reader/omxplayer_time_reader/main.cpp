//
//  main.cpp
//  omxplayer_time_reader
//
//  Created by Michael Hill on 8/3/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "NetPort.h"

int getDelimitedLine(char* line, char delim, FILE* fp)
{
    bool gotLine = false;
    int size = 0;
    while (!gotLine)
    {
        if (fread(&line[size], 1, 1, fp) != 1)
        {
            return -1;
        }
        
        if (line[size] == delim)
        {
            gotLine = true;
        }
        else
        {
            size++;
        }
    }
    
    return size;
}

int main(int argc, char * const argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    FILE* fp = stdin;
    
    int output_port = 5656;
    int input_port = 7777;
    
    int c;
    while ((c = getopt(argc, argv, "p:")) != -1)
    {
        if (c == 'p')
        {
            output_port = atoi(optarg);
        }
    }
    
    NetPort port(127,0,0,1,input_port,output_port);
    size_t linesize = 1024;
    char* line = (char*)malloc(linesize);
    while (getline(&line, &linesize, fp) > 0)
    {
        char* str = strstr(line, " M:");
        if (str)
        {
            int timeUs;
            sscanf(&str[3], "%d ", &timeUs);
            float timeSec = (float)timeUs / 1000000.0;
            printf("time %f\n", timeSec);
            
            if (timeUs < 0)
            {
                timeUs = 0;
            }
            uint32_t t = timeUs;
            uint32_t nTimeUs = htonl(t);
            port.send((unsigned char*)&nTimeUs, sizeof(uint32_t));
        }
    }
    
    return 0;
}
