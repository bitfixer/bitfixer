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
#include "c64_colors.h"

unsigned char bw_colors[] =
{
    0,      0,      0,
    255,    255,    255
};

int num_bw_colors = 2;

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
    sprintf(outfname, "%s/%s", outdir, s.c_str());
    
    printf("converting: %s", fname);
    printf("\n");
    
    Tools::Timer timer;
    Palette c64palette(c64_colors, num_64_colors);
    Image inputImage(fname);
    int imWidth = inputImage.getWidth();
    int imHeight = inputImage.getHeight();
    Image halfImage(inputImage, imWidth / 2, imHeight);
    timer.start();
    
    if (strcmp(type, "ppm") == 0)
    {
        Ditherer* ditherer = Ditherer::createC64Ditherer();
        Image* dithered = ditherer->createDitheredImageFromImageWithPalette(halfImage, c64palette);
        C64Image* c64im = (C64Image*)dithered;
        Image fullImage(*dithered, imWidth, imHeight);
        fullImage.writePPM(outfname);
        delete ditherer;
        delete dithered;
    }
    else if (strcmp(type, "c64") == 0)
    {
        Ditherer* ditherer = Ditherer::createC64Ditherer();
        Image* dithered = ditherer->createDitheredImageFromImageWithPalette(halfImage, c64palette);
        C64Image* c64im = (C64Image*)dithered;
        int c64FrameSize = c64im->getC64FrameSize();
        unsigned char* frame = (unsigned char*)malloc(sizeof(unsigned char) * c64FrameSize);
        FILE *fp = fopen(outfname, "wb");
        float time = (float)index / 4.0;
        c64im->getC64Frame(frame, time);
        fwrite(frame, 1, c64FrameSize, fp);
        fclose(fp);
        delete ditherer;
        delete dithered;
    }
    else if (strcmp(type, "bwdither") == 0)
    {
        Palette bwPalette(bw_colors, num_bw_colors);
        Ditherer* ditherer = Ditherer::createFloydSteinbergDitherer();
        Image* dithered = ditherer->createDitheredImageFromImageWithPalette(inputImage, bwPalette);
        dithered->writePPM(outfname);
        delete ditherer;
        delete dithered;
    }
    
    double time = timer.getTime();
    printf("Completed in %lf seconds.\n", time);
    return 0;
}
