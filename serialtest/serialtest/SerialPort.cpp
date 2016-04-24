//
//  SerialPort.cpp
//  serialtest
//
//  Created by Michael Hill on 3/27/16.
//  Copyright © 2016 Michael Hill. All rights reserved.
//

#include "SerialPort.hpp"
#include <iostream>
#include <string>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

SerialPort::SerialPort(const char *device, int baudrate)
{
    fd = open(device, O_RDWR | O_NDELAY | O_NOCTTY);
    if (fd == -1)
    {
        perror((string("can't open ") + device).c_str());
        exit(errno);
    }
    
    struct termios config;
    if (tcgetattr(fd, &config) < 0)
    {
        perror("can't get serial attributes");
        exit(errno);
    }
    
    if (cfsetispeed(&config, baudrate) < 0 || cfsetospeed(&config, baudrate) < 0)
    {
        perror("can't set baud rate");
        exit(errno);
    }
    
    config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
    config.c_oflag = 0;
    config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
    config.c_cflag &= ~(CSIZE | PARENB);
    config.c_cflag |= CS8 | CLOCAL;
    config.c_cc[VMIN]  = 1;
    config.c_cc[VTIME] = 0;
    
    if (tcsetattr(fd, TCSAFLUSH, &config) < 0)
    {
        perror("can't set serial attributes");
        exit(errno);
    }
}

SerialPort::~SerialPort()
{
    printf("closing serial port\n");
    if (fd != -1)
        close(fd);
}

int SerialPort::send(unsigned char *data, int length)
{
    int res = (int)write(fd, data, length);
    printf("write returned %d\n", res);
    return res;
}