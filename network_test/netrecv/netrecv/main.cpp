//
//  main.cpp
//  netrecv
//
//  Created by Michael Hill on 3/24/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#include <iostream>
#include "Netport.h"

int main(int argc, const char * argv[]) {
    printf("argc %d\n", argc);
    if (argc < 2)
    {
        printf("Usage: netrecv <r/s>\n");
        exit(0);
    }
    
    const char* dir = argv[1];
    unsigned char temp[256];
    if (strcmp(dir, "r") == 0)
    {
        // receive
        NetPort port(127,0,0,1,99999,111111);
        int ret = port.recv_sync(temp, 256);
        printf("pport got %d bytes\n", ret);
    }
    else
    {
        // send
        NetPort port(127,0,0,1,99998,99999);
        memset(temp, 1, 256);
        int ret = port.send(temp, 256);
        printf("port sent %d bytes\n", ret);
    }
    
    return 0;
}
