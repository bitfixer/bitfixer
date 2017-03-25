//
//  CommPort.h
//  serialtest
//
//  Created by Michael Hill on 4/10/16.
//  Copyright (c) 2016 Michael Hill. All rights reserved.
//

#ifndef __serialtest__CommPort__
#define __serialtest__CommPort__

class CommPort
{
public:
    CommPort() {};
    virtual ~CommPort() {};
    virtual int send(unsigned char *data, int length) = 0;
    virtual int recv(unsigned char *data, int length) = 0;
    virtual int recv_sync(unsigned char *data, int length, int timeout_ms = 200) = 0;
};

#endif /* defined(__serialtest__CommPort__) */
