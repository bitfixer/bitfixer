//
//  SerialPort.hpp
//  serialtest
//
//  Created by Michael Hill on 3/27/16.
//  Copyright © 2016 Michael Hill. All rights reserved.
//

#ifndef SerialPort_hpp
#define SerialPort_hpp

class SerialPort
{
public:
    SerialPort(const char *device, int baudrate);
    
    ~SerialPort();
    void send(unsigned char *data, int length);
    //int recv(unsigned char *data, int length);
    //int recv_sync(unsigned char *data, int length, int timeout_ms = 200);
    
private:
    int fd = -1;
};

#endif /* SerialPort_hpp */
