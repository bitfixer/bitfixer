//
//  main.cpp
//  dgtape
//
//  Created by Michael Hill on 1/4/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void writeBit(unsigned char bit, FILE* outfp, int samplesPerBit, float& phase, bool marker = false)
{
    float zeroFreq = 2975.0f;
    float oneFreq = 2125.0f;
    float sampleRate = 44100.0f;
    
    float freq = (bit == 0) ? zeroFreq : oneFreq;
    float increment = ((2.0f * M_PI) * freq) / sampleRate;
    
    for (int i = 0; i < samplesPerBit; i++)
    {
        //*buffer = sinf(phase);
        //float sample = sinf(phase);
        float sample = (marker && i==0) ? 1.0f : sinf(phase);
        fwrite(&sample, sizeof(float), 1, outfp);
        phase += increment;
    }
    
    phase = fmodf(phase, 2.0f * M_PI);
}

int main(int argc, const char * argv[]) {
    
    const char* fname = argv[1];
    printf("fname is: %s\n", fname);
    
    FILE* fp = fopen(fname, "rb");
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    printf("length is %ld\n", len);
    
    float baudrate = 1100.0f;
    float samplerate = 44100.0f;
    float bitsperbyte = 10.0f;
    float samplesperbyte = bitsperbyte * (1.0/baudrate * samplerate);
    printf("samples per byte: %f\n", samplesperbyte);
    
    // get integer number of samples
    int samplesperbit = (int)round(samplesperbyte / bitsperbyte);
    printf("samples per bit: %d\n", samplesperbit);
    
    float secondsPerBit = (float)samplesperbit / samplerate;
    float realBps = 1.0f / secondsPerBit;
    printf("real bps: %f\n", realBps);
    
    
    int samplesinprogram = samplesperbit * (int)bitsperbyte * (int)len;
    printf("samplesinprogram: %d\n", samplesinprogram);
    
    // generating audio file
    // write out samples
    FILE* outfp = fopen("out.raw", "wb");
    float sinphase = 0.0f;
    // write a header
    
    
    for (int i = 0; i < 2000; i++)
    {
        writeBit(1, outfp, samplesperbit, sinphase);
    }
    
    
    
    for (long i = 0; i < len; i++)
    {
        unsigned char byte;
        fread(&byte, 1, 1, fp);
        printf("index %ld: byte %02X\n", i, byte);
        
        // start bit
        writeBit(0, outfp, samplesperbit, sinphase);
        
        bool parity = true;
        // little endian, shift bits off
        for (int b = 0; b < 8; b++)
        {
            // get current lowest bit
            unsigned char bit = byte & 0x01;
            
            // flip parity bit, if 1
            if (bit == 1)
            {
                parity = parity ? false : true;
            }
            
            writeBit(bit, outfp, samplesperbit, sinphase);
            
            // right shift bit off byte
            byte >>= 1;
        }
        
        // assume even parity?
        
        /*
        if (parity)
        {
            writeBit(1, outfp, samplesperbit, sinphase);
        }
        else
        {
            writeBit(0, outfp, samplesperbit, sinphase);
        }
        */
        
        //writeBit(1, outfp, samplesperbit, sinphase);
         
        // stop bit
        writeBit(1, outfp, samplesperbit, sinphase, false);
    }
    
    
    // write a footer
    for (int i = 0; i < 2000; i++)
    {
        writeBit(1, outfp, samplesperbit, sinphase);
    }
    
    fclose(outfp);
    fclose(fp);
    
    return 0;
}
