//
//  TCP.h
//  tcp_server
//
//  Created by Michael Hill on 8/30/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#ifndef TCP_h
#define TCP_h

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include "CommPort.h"

class TCPServer {
public:
    TCPServer(int port) :
    _port(port)
    {};
    
    ~TCPServer() {};
    
    void init();
    
    // get new TCP connection, should be done on separate thread
    CommPort* getNewConnection();
    void close();
    
private:
    int _port;
    int _sockfd;
    struct sockaddr_in _serverAddress;
};

class TCPClient : public CommPort {
public:
    TCPClient(const char* ipaddress, int port)
    : _ipaddress(ipaddress)
    , _port(port)
    , _conn(NULL)
    {};
    
    ~TCPClient() {};
    bool connect();
    
    int send(unsigned char *data, int length)
    {
        return _conn->send(data, length);
    }
    
    int recv(unsigned char *data, int length)
    {
        return _conn->recv(data, length);
    }
    
    int recv_sync(unsigned char *data, int length, int timeout_ms = 200)
    {
        return _conn->recv_sync(data, length, timeout_ms);
    }
    
private:
    std::string _ipaddress;
    int _port;
    CommPort *_conn;
};

#endif /* TCP_h */
