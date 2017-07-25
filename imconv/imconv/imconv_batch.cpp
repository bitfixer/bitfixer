//
//  main.cpp
//  imconv
//
//  Created by Michael Hill on 2/17/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#include <iostream>
#include <math.h>
#include "Image.hpp"
#include "Ditherer.hpp"
#include "timer.hpp"
#include "C64Image.cpp"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "StringTools.h"
#include "c64_colors.h"

int main(int argc, const char * argv[]) {
    const char* type = argv[1];
    float fps = (float)atoi(argv[2]);
    FILE *fpin = stdin;
    if (argc > 3)
    {
        fpin = fopen(argv[3], "rb");
    }
    
    int index = 0;
    Tools::Timer timer;
    Palette c64palette(c64_colors, num_64_colors);
    
    int w = 320;
    int h = 200;
    int bytesInImage = w * h * 3;
    unsigned char* imageBuf = new unsigned char[bytesInImage];
    unsigned char* frame = NULL;
    Ditherer* ditherer = Ditherer::createC64Ditherer();
    while (fread(imageBuf, 1, bytesInImage, fpin) == bytesInImage)
    {
        Image inputImage(w, h, imageBuf);
        int imWidth = inputImage.getWidth();
        int imHeight = inputImage.getHeight();
        C64Image halfImage(inputImage, imWidth / 2, imHeight);
        timer.start();
        
        if (strcmp(type, "ppm") == 0)
        {
            ditherer->ditherImageInPlaceWithPalette(halfImage, c64palette);
            Image fullImage(halfImage, imWidth, imHeight);
            char temp[128];
            sprintf(temp, "out_%04d.ppm", index++);
            FILE* fp = fopen(temp, "wb");
            fullImage.writePPM(fp);
            fclose(fp);
        }
        else if (strcmp(type, "c64") == 0)
        {
            ditherer->ditherImageInPlaceWithPalette(halfImage, c64palette);
            C64Image* c64im = (C64Image*)&halfImage;
            int c64FrameSize = c64im->getC64FrameSize();
            fprintf(stderr, "c64 frame size %d\n", c64FrameSize);
            if (!frame)
            {
                frame = new unsigned char[c64FrameSize];
            }
            FILE* fp = stdout;
            float time = (float)index / (float)fps;
            fprintf(stderr, "time is %f\n", time);
            c64im->getC64Frame(frame, time);
            fwrite(frame, 1, c64FrameSize, fp);
            if (fp != stdout)
            {
                fclose(fp);
            }
            index++;
        }
        
        double time = timer.getTime();
        fprintf(stderr, "Completed in %lf seconds.\n", time);
    }
    
    delete[] imageBuf;
    delete ditherer;
    delete[] frame;
    
    return 0;
}
