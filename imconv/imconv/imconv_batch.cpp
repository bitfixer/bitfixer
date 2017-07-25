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

unsigned char bw_colors[] =
{
    0,      0,      0,
    255,    255,    255
};

int num_bw_colors = 2;

int main(int argc, const char * argv[]) {
    const char* type = argv[1];
    
    int index = 0;
    Tools::Timer timer;
    Palette c64palette(c64_colors, num_64_colors);
    
    int w = 320;
    int h = 200;
    int bytesInImage = w * h * 3;
    unsigned char* imageBuf = new unsigned char[bytesInImage];
    size_t bytes_read = fread(imageBuf, 1, bytesInImage, stdin);
    while (bytes_read == bytesInImage)
    {
        Image inputImage(w, h, imageBuf);
        int imWidth = inputImage.getWidth();
        int imHeight = inputImage.getHeight();
        Image halfImage(inputImage, imWidth / 2, imHeight);
        timer.start();
        
        if (strcmp(type, "ppm") == 0)
        {
            Ditherer* ditherer = Ditherer::createC64Ditherer();
            Image* dithered = ditherer->createDitheredImageFromImageWithPalette(halfImage, c64palette);
            Image fullImage(*dithered, imWidth, imHeight);
            fullImage.writePPM(stdout);
            delete ditherer;
            delete dithered;
        }
        else if (strcmp(type, "c64") == 0)
        {
            Ditherer* ditherer = Ditherer::createC64Ditherer();
            Image* dithered = ditherer->createDitheredImageFromImageWithPalette(halfImage, c64palette);
            C64Image* c64im = (C64Image*)dithered;
            int c64FrameSize = c64im->getC64FrameSize();
            fprintf(stderr, "c64 frame size %d\n", c64FrameSize);
            unsigned char* frame = (unsigned char*)malloc(sizeof(unsigned char) * c64FrameSize);
            FILE* fp = stdout;
            float time = (float)index / 4.0;
            fprintf(stderr, "time is %f\n", time);
            c64im->getC64Frame(frame, time);
            fwrite(frame, 1, c64FrameSize, fp);
            if (fp != stdout)
            {
                fclose(fp);
            }
            delete ditherer;
            delete dithered;
            free(frame);
            
            index++;
        }
        else if (strcmp(type, "bwdither") == 0)
        {
            Palette bwPalette(bw_colors, num_bw_colors);
            Ditherer* ditherer = Ditherer::createFloydSteinbergDitherer();
            Image* dithered = ditherer->createDitheredImageFromImageWithPalette(inputImage, bwPalette);
            dithered->writePPM(stdout);
            delete ditherer;
            delete dithered;
        }
        
        double time = timer.getTime();
        fprintf(stderr, "Completed in %lf seconds.\n", time);
        bytes_read = fread(imageBuf, 1, bytesInImage, stdin);
    }
    
    delete[] imageBuf;
    
    return 0;
}
