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
}

void NetPort::send(unsigned char *data, int length)
{
    socket.Send(destination, data, length);
}

NetPort::~NetPort()
{
    socket.Close();
}