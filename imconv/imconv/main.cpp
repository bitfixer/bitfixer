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
#include "spock.c"
#include "timer.hpp"
#include "C64Image.cpp"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "StringTools.h"

unsigned char c64_colors[] =
{
    0,      0,      0,
    255,    255,    255,
    136,    0,      0,
    170,    255,    238,
    204,    68,     204,
    0,      204,    85,
    0,      0,      170,
    238,    238,    119,
    221,    136,    85,
    102,    68,     0,
    255,    119,    119,
    51,     51,     51,
    119,    119,    119,
    170,    255,    102,
    0,      136,    255,
    187,    187,    187
};

int num_64_colors = 16;

unsigned char cga_colors[] =
{
    0,      0,      0,
    255,    255,    255,
    0x55,   0xFF,   0xFF,
    0xFF,   0x55,   0xFF
};

int num_cga_colors = 4;

int main(int argc, const char * argv[]) {
    
    const char* outdir = argv[1];
    const char* type = argv[2];
    const char* fname = argv[3];
    
    list<string> parts;
    StringTools::Split(fname, "/", parts);
    string s = parts.back();
    
    // get index of image
    int index;
    sscanf(s.c_str(), "out%04d.ppm", &index);
    printf("file: %s index %d\n", s.c_str(), index);
    
    char outfname[256];
    sprintf(outfname, "%s/%s ", outdir, s.c_str());
    
    printf("converting: %s", fname);
    printf("\n");
    
    Timer timer;
    Palette c64palette(c64_colors, num_64_colors);
    Image inputImage(fname);
    int imWidth = inputImage.getWidth();
    int imHeight = inputImage.getHeight();
    Image halfImage(inputImage, imWidth / 2, imHeight);
    Ditherer* ditherer = Ditherer::createC64Ditherer();
    
    timer.start();
    Image* dithered = ditherer->createDitheredImageFromImageWithPalette(halfImage, c64palette);
    C64Image* c64im = (C64Image*)dithered;
    
    if (strcmp(type, "ppm") == 0)
    {
        Image fullImage(*dithered, imWidth, imHeight);
        fullImage.writePPM(outfname);
    }
    else if (strcmp(type, "c64") == 0)
    {
        int c64FrameSize = c64im->getC64FrameSize();
        unsigned char* frame = (unsigned char*)malloc(sizeof(unsigned char) * c64FrameSize);
        FILE *fp = fopen(outfname, "wb");
        float time = (float)index / 4.0;
        c64im->getC64Frame(frame, time);
        fwrite(frame, 1, c64FrameSize, fp);
        fclose(fp);
    }
    
    double time = timer.getTime();
    printf("Completed in %lf seconds.\n", time);
    
    
    delete ditherer;
    delete dithered;
    return 0;
}
