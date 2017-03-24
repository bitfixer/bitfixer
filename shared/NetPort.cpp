//
//  NetPort.cpp
//  soundcontroller
//
//  Created by Michael Hill on 4/10/16.
//  Copyright (c) 2016 Michael Hill. All rights reserved.
//

#include "NetPort.h"

NetPort::NetPort(unsigned char a, unsigned char b, unsigned char c, unsigned char d, int inport, int outport) :
port(inport),
destination(a, b, c, d, outport)
{
    socket.Open(port);
    timer.start();
}

int NetPort::send(unsigned char *data, int length)
{
    bool res = socket.Send(destination, data, length);
    if (res)
    {
        return length;
    }
    else
    {
        return -1;
    }
}

int NetPort::recv(unsigned char *data, int length)
{
    net::Address addr;
    int ret = socket.Receive(addr, data, length);
    
    printf("addr %d %d %d %d : %d\n", addr.GetA(), addr.GetB(), addr.GetC(), addr.GetD(), ret);
    return ret;
}

int NetPort::recv_sync(unsigned char *data, int length, int timeout_ms)
{
    net::Address addr;
    int ret = 0;
    while (ret != length)
    {
        ret = socket.Receive(addr, data, length);
        if (ret != length)
        {
            net::Tools::waitSeconds(0.1);
        }
    }
    
    printf("addr %d %d %d %d port %d : %d\n", addr.GetA(), addr.GetB(), addr.GetC(), addr.GetD(), addr.GetPort(), ret);
    return ret;
}

double NetPort::getTime()
{
    return timer.getTime();
}

NetPort::~NetPort()
{
    socket.Close();
}
