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
#include "spock_160x200.c"

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

unsigned char test_colors[] =
{
    0,      0,      0,
    51,     51,     51,
    0,      204,    85,
    0,      0,      170
};

int num_test_colors = 4;

float* createGrayscaleFromRGB(unsigned char* RGB, int width, int height)
{
    float* gray = (float*)malloc(sizeof(float) * width * height);
    
    float* gpix = gray;
    unsigned char *rgbpix = RGB;
    float scale = sqrtf(3.0);
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            float r = float(*rgbpix) / 255.0;
            rgbpix++;
            float g = float(*rgbpix) / 255.0;
            rgbpix++;
            float b = float(*rgbpix) / 255.0;
            rgbpix++;
            *gpix = sqrtf(r*r + g*g + b*b) / scale;
            //printf("%d %d %f\n", w, h, *gpix);
            //*gpix = g;
            gpix++;
        }
    }
    
    return gray;
}

float* createFloatRGBFromUnsignedChar(unsigned char* RGB, int width, int height)
{
    float* rgb_float = (float*)malloc(sizeof(float) * 3 * width * height);
    unsigned char* rgbpix = RGB;
    float* rgbfpix = rgb_float;
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            for (int c = 0; c < 3; c++)
            {
                *(rgbfpix++) = float(*(rgbpix++)) / 255.0;
            }
        }
    }
    
    return rgb_float;
}

unsigned char* createUnsignedCharRGBFromFloat(float* rgbfloat, int width, int height)
{
    unsigned char* rgb = (unsigned char*)malloc(sizeof(unsigned char) * 3 * width * height);
    
    unsigned char* rgbpix = rgb;
    float* rgbfpix = rgbfloat;
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            for (int c = 0; c < 3; c++)
            {
                *(rgbpix++) = (unsigned char)(*(rgbfpix++) * 255.0);
            }
        }
    }
    return rgb;
}

unsigned char* createRGBFromGrayscale(float* grayscale, int width, int height)
{
    unsigned char* rgb = (unsigned char*)malloc(sizeof(unsigned char) * 3 * width * height);
    
    unsigned char* rgbpix = rgb;
    float* gpix = grayscale;
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            unsigned char val = (unsigned char)(*gpix * 254.99);
            
            gpix++;
            *rgbpix = val;
            rgbpix++;
            *rgbpix = val;
            rgbpix++;
            *rgbpix = val;
            rgbpix++;
        }
    }
    return rgb;
}

float* createDitheredBWFromGrayscale(float* grayscale, int width, int height)
{
    float* dithered = (float*)malloc(sizeof(float) * width * height);
    
    // first copy the old image
    memcpy(dithered, grayscale, sizeof(float)*width*height);
    
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            // decide color for this pixel
            float thispix = dithered[h*width + w];
            float dpix = (thispix < 0.5) ? 0.0 : 1.0;
            float error = thispix - dpix;
            
            // transfer error
            dithered[h*width     + w + 1] += (error * 7.0 / 16.0);
            dithered[(h+1)*width + w - 1] += (error * 3.0 / 16.0);
            dithered[(h+1)*width + w    ] += (error * 5.0 / 16.0);
            dithered[(h+1)*width + w + 1] += (error * 1.0 / 16.0);
            
            // set pixel
            dithered[h*width + w] = dpix;
        }
    }
    
    return dithered;
}

int getClosestPaletteColorFromColor(float* incolor, float* pcolor, int* palette_indices, int num_palette_indices)
{
    float leastError = 999999.9;
    int leastErrorIndex = -1;
    
    float r_palette[3];
    float error[3];
    for (int cc = 0; cc < num_palette_indices; cc++)
    {
        int pindex = palette_indices[cc];
        float thisError = 0;
        for (int i = 0; i < 3; i++)
        {
            r_palette[i] = (float)(c64_colors[pindex*3 + i] / 255.0);
            error[i] = incolor[i] - r_palette[i];
            thisError += (error[i]*error[i]);
        }
        
        if (thisError < leastError)
        {
            leastError = thisError;
            leastErrorIndex = pindex;
        }
    }
    
    if (leastErrorIndex >= 0)
    {
        // copy palette color
        for (int i = 0; i < 3; i++)
        {
            pcolor[i] = (float)(c64_colors[leastErrorIndex*3 + i] / 255.0);
        }
    }
    
    return leastErrorIndex;
}

float* createDitheredRGBFromRGB(float* rgb, int width, int height, int* palette_indices = NULL, int* block_colors = NULL)
{
    float* ditheredrgb = (float*)malloc(sizeof(float) * 3 * width * height);
    
    int blocks[2];
    blocks[0] = width / 8;
    blocks[1] = height / 8;
    
    // first copy the old image
    memcpy(ditheredrgb, rgb, sizeof(float)*3*width*height);
    float oldrgb[3];
    float newrgb[3];
    float error[3];
    
    int* pindices = (int*)malloc(sizeof(int) * num_64_colors);
    for (int p = 0; p < num_64_colors; p++)
    {
        pindices[p] = p;
    }
    
    // TEST
    int bbcc[2];
    bbcc[0] = 0;
    bbcc[1] = 1;
    
    for (int h = 0; h < height; h++)
    {
        int hblock = h / 8;
        for (int w = 0; w < width; w++)
        {
            int wblock = w / 8;
            for (int i = 0; i < 3; i++)
            {
                oldrgb[i] = ditheredrgb[h*width*3 + w*3 + i];
            }
            
            int pindex = -1;
            if (block_colors)
            {
                pindex = getClosestPaletteColorFromColor(oldrgb, newrgb, &block_colors[hblock*blocks[0]*2 + wblock*2], 2);
                //pindex = getClosestPaletteColorFromColor(oldrgb, newrgb, bbcc, 2);
            }
            else
            {
                pindex = getClosestPaletteColorFromColor(oldrgb, newrgb, pindices, num_64_colors);
            }
            
            for (int i = 0; i < 3; i++)
            {
                error[i] = oldrgb[i] - newrgb[i];
            }
            
            for (int i = 0; i < 3; i++)
            {
                ditheredrgb[h*width*3     + (w + 1)*3 + i] += (error[i] * 7.0 / 16.0);
                
                if (h < height-1)
                {
                    ditheredrgb[(h+1)*width*3 + (w - 1)*3 + i] += (error[i] * 3.0 / 16.0);
                    ditheredrgb[(h+1)*width*3 + w*3       + i] += (error[i] * 5.0 / 16.0);
                    ditheredrgb[(h+1)*width*3 + (w + 1)*3 + i] += (error[i] * 1.0 / 16.0);
                }
                
                // set new color value
                ditheredrgb[h*width*3     + w*3       + i] = newrgb[i];
            }
            
            if (palette_indices)
            {
                palette_indices[h*width + w] = pindex;
            }
        }
    }
    
    free(pindices);
    return ditheredrgb;
}

float* createDitheredBlockRGBFromRGB(float* rgb, int width, int height)
{
    // first, do standard dither on image
    int* palette_indices = (int*)malloc((sizeof(int) * width * height));
    float *rgb_dithered = createDitheredRGBFromRGB(rgb, width, height, palette_indices);
    
    // for each block, pick the most common dither colors
    int blocks[2];
    blocks[0] = width / 8;
    blocks[1] = height / 8;
    
    int* block_colors = (int*)malloc(sizeof(int) * 2 * blocks[0] * blocks[1]);
    
    // determine block colors
    int* color_votes = (int*)malloc(sizeof(int) * num_64_colors);
    for (int hb = 0; hb < blocks[1]; hb++)
    {
        for (int wb = 0; wb < blocks[0]; wb++)
        {
            memset(color_votes, 0, sizeof(int)*num_64_colors);
            
            // tally dither colors in this block
            for (int hh = 0; hh < 8; hh++)
            {
                for (int ww = 0; ww < 8; ww++)
                {
                    int hstart = hb*8 + hh;
                    int wstart = wb*8 + ww;
                    
                    int pcolor = palette_indices[hstart*width + wstart];
                    color_votes[pcolor]++;
                }
            }
            
            // DEBUG
            for (int vv = 0; vv < num_64_colors; vv++)
            {
                printf("color_votes %d block %d,%d: %d\n", vv, wb, hb, color_votes[vv]);
            }
            
            // now take the 2 highest vote counts
            for (int cc = 0; cc < 2; cc++)
            {
                int highestVotes = -1;
                int highestVoteIndex = -1;
                for (int i = 0; i < num_64_colors; i++)
                {
                    if (color_votes[i] > highestVotes)
                    {
                        highestVotes = color_votes[i];
                        highestVoteIndex = i;
                    }
                }
                
                block_colors[hb*blocks[0]*2 + wb*2 + cc] = highestVoteIndex;
                color_votes[highestVoteIndex] = 0;
            }
        }
    }

    // now run the dither again with the specified block colors
    free(rgb_dithered);
    rgb_dithered = createDitheredRGBFromRGB(rgb, width, height, palette_indices, block_colors);
    
    free(block_colors);
    free(color_votes);
    
    return rgb_dithered;
}

void writeImage(const char* fname, unsigned char* rgb, int width, int height)
{
    FILE* fp = fopen(fname, "wb");
    
    // write ppm header
    fprintf(fp, "P6 %d %d 255\n", width, height);
    // write pixels
    fwrite(rgb, 1, 3*width*height, fp);
    fclose(fp);
}

int main(int argc, const char * argv[]) {
    
    Palette c64palette(c64_colors, num_64_colors);
    Palette cgaPalette(cga_colors, num_cga_colors);
    Palette testPalette(test_colors, num_test_colors);
    
    Image spockImage(gimp_image.width, gimp_image.height, (unsigned char*)gimp_image.pixel_data);
    //spockImage.writePPM("spockout.ppm");
    
    //Ditherer* fsDitherer = Ditherer::createNearestNeighborDitherer();
    //Ditherer* fsDitherer = Ditherer::createFloydSteinbergDitherer();
    Ditherer* fsDitherer = Ditherer::createC64Ditherer();
    //Image* dithered = fsDitherer->createDitheredImageFromImageWithPalette(spockImage, c64palette);
    //dithered->colorHistogram();
    
    //Image* dithered = fsDitherer->createDitheredImageFromImageWithPalette(spockImage, cgaPalette);
    Image* dithered = fsDitherer->createDitheredImageFromImageWithPalette(spockImage, c64palette);
    //Image* dithered = fsDitherer->createDitheredImageFromImageWithPalette(spockImage, testPalette);
    dithered->writePPM("spock_bb.ppm");
    //dithered->colorHistogram();
    
    
    /*
    dithered->writePPM("s_dithered_3.ppm");
    
    Ditherer* nnDitherer = Ditherer::createNearestNeighborDitherer();
    Image* nnImage = nnDitherer->createDitheredImageFromImageWithPalette(spockImage, c64palette);
    nnImage->writePPM("s_nn.ppm");
    
     
    delete fsDitherer;
    delete nnDitherer;
    */
    delete dithered;
    
    /*
    int numBytes = gimp_image.height * gimp_image.width * gimp_image.bytes_per_pixel;
    
    float *rgbf = createFloatRGBFromUnsignedChar((unsigned char*)gimp_image.pixel_data, gimp_image.width, gimp_image.height);
    unsigned char* rgb = createUnsignedCharRGBFromFloat(rgbf, gimp_image.width, gimp_image.height);
    
    writeImage("spockcolor.ppm", rgb, gimp_image.width, gimp_image.height);
    
    float *rgb_dithered = createDitheredRGBFromRGB(rgbf, gimp_image.width, gimp_image.height);
    unsigned char* rgb_d_uchar = createUnsignedCharRGBFromFloat(rgb_dithered, gimp_image.width, gimp_image.height);
    writeImage("spock_dithered_rgb.ppm", rgb_d_uchar, gimp_image.width, gimp_image.height);
    
    
    float *rgb_bd = createDitheredBlockRGBFromRGB(rgbf, gimp_image.width, gimp_image.height);
    unsigned char* rgb_bd_uchar = createUnsignedCharRGBFromFloat(rgb_bd, gimp_image.width, gimp_image.height);
    writeImage("spock_bd_rgb.ppm", rgb_bd_uchar, gimp_image.width, gimp_image.height);
    
    free(rgbf);
    free(rgb);
    */
     
    return 0;
}
