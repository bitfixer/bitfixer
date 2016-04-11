//
//  SidController.h
//  serialtest
//
//  Created by Michael Hill on 4/10/16.
//  Copyright (c) 2016 Michael Hill. All rights reserved.
//

#ifndef __serialtest__SidController__
#define __serialtest__SidController__

#include <stdio.h>
#include "CommPort.h"
#include "sid.h"

void send_control_packet(CommPort *port, unsigned char type, unsigned char offset, unsigned char val);

class SidController : public Sid
{
public:
    SidController(CommPort *p);
    void init();
    void update();
    
private:
    unsigned char type = 1;
    CommPort *port = NULL;
    struct __sid sid_state;
};

#endif /* defined(__serialtest__SidController__) */
