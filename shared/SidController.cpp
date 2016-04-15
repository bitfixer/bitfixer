//
//  SidController.cpp
//  serialtest
//
//  Created by Michael Hill on 4/10/16.
//  Copyright (c) 2016 Michael Hill. All rights reserved.
//

#include <string.h>
#include "SidController.h"

int SidController::send_control_packet(CommPort *port, unsigned char type, unsigned char offset, unsigned char val, unsigned char *dest)
{
    printf("sending: %d %d %d\n", type, offset, val);
    int bytes_sent = 0;
    dest[bytes_sent++] = offset;
    dest[bytes_sent++] = val;
    return bytes_sent;
    
    
    //port->send(&offset, 1);
    //port->send(&val, 1);
}

SidController::SidController(CommPort *p) :
port(p)
{
    // create sid class
}

void SidController::init()
{
    Sid::init();
    
    memset((void *)&sid_state, 255, sizeof(struct __sid));
    update();
}

void SidController::update()
{
    unsigned char *buf = buffer;
    int packet_bytes = 0;
    
    int numBytes = sizeof(struct __sid);
    unsigned char *siddata = (unsigned char *)getData();
    unsigned char *state = (unsigned char *)&sid_state;
    
    for (int i = 0; i < numBytes; i++)
    {
        if (state[i] != siddata[i])
        {
            int bytes_sent = send_control_packet(port, type, i, siddata[i], buf);
            buf += bytes_sent;
            packet_bytes += bytes_sent;
        }
        state[i] = siddata[i];
    }
    
    // send update packet
    if (packet_bytes > 0)
        port->send(buffer, packet_bytes);
}
