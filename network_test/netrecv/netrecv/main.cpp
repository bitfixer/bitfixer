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
        // read specified file
        if (argc >= 3)
        {
            const char* fname = argv[2];
            FILE* fp = fopen(fname, "rb");
            fseek(fp, 0, SEEK_END);
            int size = (int)ftell(fp);
            fseek(fp, 0, SEEK_SET);
            printf("%s is %d bytes\n", fname, size);
            
            unsigned char* buf = (unsigned char*)malloc(size);
            fread(buf, 1, size, fp);
            fclose(fp);
            
            // skip first 4 bytes
            
            NetPort port(192,168,1,25,99998,99999);
            int ret = port.send(&buf[4], size-4);
            printf("port sent %d bytes from file %s\n", ret, fname);
        }
        else
        {
            unsigned char buf[9216];
            int size = 9216;
            NetPort port(192,168,1,25,99998,99999);
            memset(buf, 1, size);
            int ret = port.send(buf, size);
            printf("port sent %d bytes\n", ret);
        }
    }
    
    return 0;
}
