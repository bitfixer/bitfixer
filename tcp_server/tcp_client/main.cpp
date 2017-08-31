//
//  main.cpp
//  tcp_client
//
//  Created by Michael Hill on 8/30/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#include <iostream>
#include "TCP.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    unsigned char buffer[256];
    TCPClient client("127.0.0.1", 44444);
    
    if (!client.connect())
    {
        printf("failed to connect!\n");
        return 0;
    }
    
    memset(buffer, 0, 256);
    for (int i = 0; i < 10; i++)
    {
        buffer[0] = i;
        client.send(buffer, 1);
        sleep(1);
    }
    
    printf("done.\n");
}
