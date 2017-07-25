//
//  main.cpp
//  netrecv
//
//  Created by Michael Hill on 3/24/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Netport.h"
#include "timer.hpp"

int main(int argc, const char * argv[]) {
    int c;
    bool send = false;
    int inport = -1;
    int outport = -1;
    int chunksize = 9216;
    int ipaddr[4] = {127, 0, 0, 1};
    int numpackets = -1;
    FILE* fpin = stdin;
    
    while ((c = getopt(argc, (char**)argv, "sra:i:o:f:c:n:")) != -1)
    {
        if (c == 'a')
        {
            // parse address
            sscanf(optarg, "%d.%d.%d.%d", &ipaddr[0], &ipaddr[1], &ipaddr[2], &ipaddr[3]);
            printf("address: %d %d %d %d\n", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
        }
        else if (c == 'i')
        {
            // parse input port
            inport = atoi(optarg);
            printf("input port: %d\n", inport);
            
        }
        else if (c == 'o')
        {
            // parse output port
            outport = atoi(optarg);
            printf("output port: %d\n", outport);
        }
        else if (c == 's')
        {
            send = true;
            printf("send\n");
        }
        else if (c == 'f')
        {
            fpin = fopen(optarg, "rb");
        }
        else if (c == 'c')
        {
            chunksize = atoi(optarg);
        }
        else if (c == 'n')
        {
            numpackets = atoi(optarg);
        }
        else
        {
            abort();
        }
    }
    
    unsigned char* buf = new unsigned char[chunksize];
    NetPort port(ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3],inport,outport);
    Tools::Timer timer;
    timer.start();
    int currpkt = 0;
    if (!send)
    {
        while (numpackets == -1 || currpkt < numpackets)
        {
            int ret = port.recv_sync(buf, chunksize);
            printf("pport got %d bytes time %lf\n", ret, timer.getTime());
            currpkt++;
        }
    }
    else
    {
        while ((numpackets == -1 || currpkt < numpackets) &&
               fread(buf, 1, chunksize, fpin) == chunksize)
        {
            int ret = port.send(buf, chunksize);
            printf("port sent %d bytes\n", ret);
            currpkt++;
        }
    }
    
    delete[] buf;
    if (fpin != stdin)
    {
        fclose(fpin);
    }
    
    return 0;
}
