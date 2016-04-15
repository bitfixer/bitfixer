//
//  NetPort.h
//  soundcontroller
//
//  Created by Michael Hill on 4/10/16.
//  Copyright (c) 2016 Michael Hill. All rights reserved.
//

#ifndef __NetPort__
#define __NetPort__

#include "CommPort.h"
#include "Net.h"
#include "timer.hpp"

class NetPort : public CommPort
{
public:
    typedef struct
    {
        double time;
        double sendTime;
        int unique_id;
        unsigned char buffer[256];
    } TimedPacket;
    
    NetPort(unsigned char a, unsigned char b, unsigned char c, unsigned char d, int port);
    ~NetPort();
    int send(unsigned char *data, int length);
    double getTime();
    
    double delay = 0.0;
private:
    net::Socket socket;
    net::Address destination;
    unsigned char addr[4];
    Timer timer;
    TimedPacket pkt;
    int port_id = -1;
    int port;
};

#endif /* defined(__soundcontroller__NetPort__) */
