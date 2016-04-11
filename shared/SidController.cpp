//
//  SidController.cpp
//  serialtest
//
//  Created by Michael Hill on 4/10/16.
//  Copyright (c) 2016 Michael Hill. All rights reserved.
//

#include <string.h>
#include "SidController.h"

void send_control_packet(CommPort *port, unsigned char type, unsigned char offset, unsigned char val)
{
    printf("sending: %d %d %d\n", type, offset, val);
    
    //port->send(&type, 1);
    port->send(&offset, 1);
    port->send(&val, 1);
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
    int numBytes = sizeof(struct __sid);
    unsigned char *siddata = (unsigned char *)getData();
    unsigned char *state = (unsigned char *)&sid_state;
    
    for (int i = 0; i < numBytes; i++)
    {
        if (state[i] != siddata[i])
        {
            send_control_packet(port, type, i, siddata[i]);
        }
        state[i] = siddata[i];
    }
}
