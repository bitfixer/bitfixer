//
//  main.cpp
//  binaryDataConvert
//
//  Created by Michael Hill on 4/15/16.
//  Copyright (c) 2016 Michael Hill. All rights reserved.
//

#include <iostream>
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    
    
    FILE *fp = fopen(argv[1], "rb");
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    FILE *out = fopen(argv[2], "wb");
    unsigned char temp[1024];
    sprintf((char *)temp, (char *)"unsigned char data[] = {\n\t");
    fwrite((char *)temp, strlen((char *)temp), 1, out);
    
    for (long b = 0; b < size; b++)
    {
        unsigned char data;
        fread(&data, 1, 1, fp);
        fprintf(out, "0x%02X", data);
        if (b < size-1)
        {
            fprintf(out, ",");
        }
        
        if (b % 40 == 39)
        {
            fprintf(out, "\n\t");
        }
    }
    
    fprintf(out, "\n};\n");
    
    return 0;
}
