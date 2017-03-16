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
    C64Image* c64im = (C64Image*)dithered;
    
    /*
    for (int y = 0; y < c64im->getYBlocks(); y++)
    {
        for (int x = 0; x < c64im->getXBlocks(); x++)
        {
            int c1 = c64im->getBlockColor(x, y, 0);
            int c2 = c64im->getBlockColor(x, y, 1);
            
            printf("block %d %d: colors %d %d\n", x, y, c1, c2);
        }
    }
    printf("bgcolor %d fgcolor %d\n", c64im->bgcolor, c64im->fgcolor);
    */
    
    int sbsize = c64im->getScreenBytesSize();
    unsigned char* sb = (unsigned char*)malloc(sizeof(unsigned char) * sbsize);
    c64im->getScreenBytes(sb);
    
    FILE *fp = fopen("screenbytes.c64", "wb");
    
    for (int i = 0; i < 30; i++)
    {
        float time = (float)i * 1.0/4.0;
        unsigned char f = 0x00;
        int rem = 8192 - sbsize;
        fwrite(&time, 1, sizeof(float), fp);
        for (int j = 0; j < 1024; j++)
        {
            fwrite(&f, 1, 1, fp);
        }
        fwrite(sb, 1, sbsize, fp);
        
        for (int j = 0; j < rem; j++)
        {
            fwrite(&f, 1, 1, fp);
        }
    }
    
    fclose(fp);
    
    
    double time = timer.getTime();
    
    Image fullImage(*dithered, imWidth, imHeight);
    printf("Completed in %lf seconds.\n", time);
    fullImage.writePPM(argv[2]);
    
    delete ditherer;
    delete dithered;
    return 0;
}
