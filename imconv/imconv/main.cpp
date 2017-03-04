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
    
    printf("converting %s\n", argv[1]);
    Timer timer;
    Palette c64palette(c64_colors, num_64_colors);
    Image inputImage(argv[1]);
    int imWidth = inputImage.getWidth();
    int imHeight = inputImage.getHeight();
    Image halfImage(inputImage, imWidth / 2, imHeight);
    Ditherer* ditherer = Ditherer::createC64Ditherer();
    
    timer.start();
    Image* dithered = ditherer->createDitheredImageFromImageWithPalette(halfImage, c64palette);
    double time = timer.getTime();
    
    Image fullImage(*dithered, imWidth, imHeight);
    printf("Completed in %lf seconds.\n", time);
    //fullImage.writePPM(argv[2]);
    fullImage.writePPM(argv[2]);
    
    delete ditherer;
    delete dithered;
    return 0;
}
