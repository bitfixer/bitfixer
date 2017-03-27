//
//  main.cpp
//  batchc64convert
//
//  Created by Michael Hill on 3/21/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#include <iostream>
#include "Image.hpp"
#include "Ditherer.hpp"
#include "C64Image.hpp"
#include "c64_colors.h"
#include "NetPort.h"
#include <unistd.h>

void wait_for_file(const char* fname)
{
    FILE* fp = NULL;
    while (fp == NULL)
    {
        fp = fopen(fname, "rb");
        if (!fp)
        {
            usleep(10000);
        }
    }
    
    // file exists now
    fclose(fp);
}

int main(int argc, const char * argv[]) {
    // insert code here...
    //std::cout << "Hello, World!\n";
    
    Palette c64palette(c64_colors, num_64_colors);
    Ditherer* ditherer = Ditherer::createC64Ditherer();
    
    const char* dirname = argv[1];
    const char* outdirname = argv[2];
    const char* out64fname = argv[3];
    
    int index = 0;
    char thisfname[256];
    char nextfname[256];
    char outfname[256];
    unsigned char* c64frame = NULL;
    
    FILE* fp = fopen(out64fname, "wb");
    NetPort port(192,168,1,25,99998,99999);
    Tools::Timer frameTimer;
    
    frameTimer.start();
    for (index = 1; index < 9999; index++)
    {
        // check if file can be opened
        sprintf(thisfname, "%s/%04d.ppm", dirname, index);
        sprintf(nextfname, "%s/%04d.ppm", dirname, index+1);
        
        wait_for_file(nextfname);
        printf("%s\n", thisfname);
        
        // open image
        Image inputImage(thisfname);
        int imWidth = inputImage.getWidth();
        int imHeight = inputImage.getHeight();
        Image halfImage(inputImage, imWidth/2, imHeight);
        
        Image* dithered = ditherer->createDitheredImageFromImageWithPalette(halfImage, c64palette);
        C64Image* c64im = (C64Image*)dithered;
        
        /*
        // test - output to new ppm
        sprintf(outfname, "%s/%04d.ppm", outdirname, index);
        Image fullImage(*dithered, imWidth, imHeight);
        fullImage.writePPM(outfname);
        */
        
        int c64FrameSize = c64im->getC64FrameSize();
        if (!c64frame)
        {
            c64frame = (unsigned char*)malloc(sizeof(unsigned char) * c64FrameSize);
        }
        float time = (float)index / 8.0;
        c64im->getC64Frame(c64frame, time);
        fwrite(c64frame, 1, c64FrameSize, fp);
        
        float thisTime = frameTimer.getTime();
        printf("pts %f, time %f\n", time, thisTime);
        while (thisTime < time)
        {
            float diff = time - thisTime;
            float usec = diff * 1000000.0;
            usleep(usec);
            thisTime = frameTimer.getTime();
        }
        // send bytes to port
        port.send(&c64frame[4], 9216);
        
        delete dithered;
    }
    fclose(fp);
    
    return 0;
}
