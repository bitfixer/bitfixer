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

class NetPort : public CommPort
{
public:
    NetPort(unsigned char a, unsigned char b, unsigned char c, unsigned char d, int port);
    ~NetPort();
    void send(unsigned char *data, int length);
private:
    net::Socket socket;
    net::Address destination;
    unsigned char addr[4];
    int port;
};

#endif /* defined(__soundcontroller__NetPort__) */
