//
//  main.cpp
//  petnetplayer
//
//  Created by Michael Hill on 4/1/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#include <iostream>
#include "Netport.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    
    const char* fname = argv[1];
    printf("file %s\n", fname);
    FILE* fp = fopen(fname, "rb");
    
    NetPort port(192,168,1,27,9998,9999);
    
    bool done = false;
    unsigned char buffer[1024];
    while (!done)
    {
        size_t res = 0;
        res = fread(buffer, 1, 1024, fp);
        if (res == 1024)
        {
            port.send(buffer, 1000);
            usleep(66666);
        }
        else
        {
            done = true;
        }
    }
    
    fclose(fp);
    return 0;
}
