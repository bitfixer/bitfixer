//
//  TCP.cpp
//  tcp_server
//
//  Created by Michael Hill on 8/30/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#include "TCP.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class TCPConnection : public CommPort
{
public:
    TCPConnection(int sockfd)
    : _sockfd(sockfd)
    {};
    
    ~TCPConnection()
    {
        fprintf(stderr, "closing socket %d\n", _sockfd);
        close(_sockfd);
    };
    
    int send(unsigned char *data, int length)
    {
        int n = (int)write(_sockfd, data, length);
        if (n < 0)
        {
            fprintf(stderr, "error in send\n");
        }
        
        return n;
    }
    
    int recv(unsigned char *data, int length)
    {
        int n = (int)read(_sockfd, data, length);
        if (n < 0)
        {
            fprintf(stderr, "error in recv\n");
        }
        
        return n;
    }
    
    int recv_sync(unsigned char *data, int length, int timeout_ms = 200)
    {
        return recv(data, length);
    }
    
private:
    int _sockfd;
};


void TCPServer::init()
{
    // create socket
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd < 0)
    {
        fprintf(stderr, "Error opening socket.\n");
        return;
    }
    
    memset(&_serverAddress, 0, sizeof(_serverAddress));
    _serverAddress.sin_family = AF_INET;
    _serverAddress.sin_addr.s_addr = INADDR_ANY;
    _serverAddress.sin_port = htons(_port);
    
    if (bind(_sockfd, (struct sockaddr*)&_serverAddress, sizeof(_serverAddress)) < 0)
    {
        fprintf(stderr, "Error binding socket.\n");
        return;
    }
    
    listen(_sockfd, 5);
}

CommPort* TCPServer::getNewConnection()
{
    // wait for client to connect
    struct sockaddr_in clientAddress;
    int clientLength = sizeof(clientAddress);
    
    int newsockfd = accept(_sockfd, (struct sockaddr*)&clientAddress, (socklen_t*)&clientLength);
    if (newsockfd < 0)
    {
        fprintf(stderr, "error on accept\n");
        return NULL;
    }
    
    printf("got connection from %s\n", inet_ntoa(clientAddress.sin_addr));
    return new TCPConnection(newsockfd);
}

void TCPServer::close()
{
    ::close(_sockfd);
}

// client

bool TCPClient::connect()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        fprintf(stderr, "error creating socket.\n");
        return false;
    }
    
    struct sockaddr_in serverAddress;
    struct hostent* server;
    server = gethostbyname(_ipaddress.c_str());
    if (server == NULL)
    {
        fprintf(stderr, "error getting address: %s\n", _ipaddress.c_str());
        return false;
    }
    
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    memcpy((char*)&serverAddress.sin_addr.s_addr, server->h_addr, server->h_length);
    serverAddress.sin_port = htons(_port);
    
    if (::connect(sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
    {
        fprintf(stderr, "error in connect.\n");
        return false;
    }
    
    _conn = new TCPConnection(sockfd);
    return true;
}

