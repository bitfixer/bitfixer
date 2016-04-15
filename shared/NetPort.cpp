//
//  NetPort.cpp
//  soundcontroller
//
//  Created by Michael Hill on 4/10/16.
//  Copyright (c) 2016 Michael Hill. All rights reserved.
//

#include "NetPort.h"

NetPort::NetPort(unsigned char a, unsigned char b, unsigned char c, unsigned char d, int port) :
port(port),
destination(a, b, c, d, port)
{
    socket.Open(port);
    timer.start();
}

int NetPort::send(unsigned char *data, int length)
{
    /*
    if (port_id == -1)
    {
        double t = timer.getTime();
        port_id = (int)(t * 1000000000.0);
        printf("port id %d\n", port_id);
    }
    
    memcpy(pkt.buffer, data, length);
    int size = sizeof(pkt) - sizeof(pkt.buffer) + length;
    pkt.time = timer.getTime();
    pkt.sendTime = pkt.time + delay;
    pkt.unique_id = port_id;
    
    bool res = socket.Send(destination, (void *)&pkt, size);
    printf("sent %d byte payload time %lf sendTime %lf\n", length, pkt.time, pkt.sendTime);
    
    if (res)
    {
        return length;
    }
    else
    {
        return -1;
    }
    */
    
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

double NetPort::getTime()
{
    return timer.getTime();
}

NetPort::~NetPort()
{
    socket.Close();
}