//
//  main.cpp
//  audioserialterminal
//
//  Created by Michael Hill on 1/27/16.
//  Copyright © 2016 jaunt. All rights reserved.
//

#include <iostream>
#include "AudioSerial.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    AudioSerialPort port(48000.0, 9600.0);
    //unsigned char str[10000];
    //sprintf((char *)str, "once upon a time there was a cool dude. 12345678");
    
    /*
    // TEST
    FILE *fpin = fopen("input.txt", "rb");
    fseek(fpin, 0, SEEK_END);
    long len = ftell(fpin);
    fseek(fpin, 0, SEEK_SET);
    
    unsigned char *input = (unsigned char *)malloc(sizeof(unsigned char) * len);
    fread(input, sizeof(unsigned char), len, fpin);
    fclose(fpin);
    
    port.send(input, (int)len);
    
     
    // TEST
    
    FILE *fp = fopen("test.raw", "wb");
    float samples[1024];
    
    for (int i = 0; i < 100; i++)
    {
        port.getaudio(samples, 1024);
        port.readaudio(samples, 1024);
        fwrite(samples, sizeof(float), 1024, fp);
    }
    fclose(fp);
    */
    
    FILE *fpin = fopen("recv2.raw", "rb");
    fseek(fpin, 0, SEEK_END);
    long len = ftell(fpin);
    fseek(fpin, 0, SEEK_SET);
    int numfloats = (int)len / sizeof(float);
    
    float *recv = (float *)malloc(sizeof(float) * numfloats);
    fread(recv, sizeof(float), numfloats, fpin);
    fclose(fpin);
    
    port.readaudio(recv, numfloats);
    
    
    
    return 0;
}
