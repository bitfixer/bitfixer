//
//  main.cpp
//  c64netplayer
//
//  Created by Michael Hill on 4/1/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include "Netport.h"


int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    const char* fname = argv[1];
    FILE* fp = fopen(fname, "rb");
    
    NetPort port(192,168,1,6,99998,99999);
    
    bool done = false;
    unsigned char buffer[9216];
    while (!done)
    {
        float time = 0.0;
        size_t res = 0;
        res = fread(&time, sizeof(float), 1, fp);
        res = fread(buffer, 1, 9216, fp);
        
        if (res == 9216)
        {
            port.send(buffer, 9216);
            usleep(250000);
        }
        else
        {
            done = true;
        }
    }
    
    
    return 0;
}
