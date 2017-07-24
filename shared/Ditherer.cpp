//
//  Ditherer.cpp
//  imconv
//
//  Created by Michael Hill on 2/20/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#include "Ditherer.hpp"
#include "C64Image.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <mutex>
#include <thread>

using std::vector;
using std::mutex;

class FloydSteinbergDitherer : public Ditherer
{
public:
    Image* createDitheredImageFromImageWithPalette(const Image& image, const Palette& palette);
    void ditherImageInPlaceWithPalette(const Image& image, const Palette& palette);
};

Image* FloydSteinbergDitherer::createDitheredImageFromImageWithPalette(const Image& image, const Palette& palette)
{
    // create copy of image
    Image* dImage = new Image(image);
    
    Color pixelColor;
    Color paletteColor;
    //int pColorIndex = -1;
    for (int h = 0; h < dImage->getHeight(); h++)
    {
        for (int w = 0; w < dImage->getWidth(); w++)
        {
            Pixel* p = dImage->pixelAt(w, h);
            pixelColor.fromPixel(*p);
            palette.getClosestColorTo(pixelColor, paletteColor, p->palette_index);
            
            for (int c = 0; c < 3; c++)
            {
                float diff = pixelColor.rgb[c] - paletteColor.rgb[c];
                
                Pixel *pp;
                pp = dImage->pixelAt(w+1, h);
                if (pp) pp->rgb[c] += diff * 7.0 / 16.0;
                pp = dImage->pixelAt(w-1, h+1);
                if (pp) pp->rgb[c] += diff * 3.0 / 16.0;
                pp = dImage->pixelAt(w, h+1);
                if (pp) pp->rgb[c] += diff * 5.0 / 16.0;
                pp = dImage->pixelAt(w+1, h+1);
                if (pp) pp->rgb[c] += diff * 1.0 / 16.0;
                
                p->rgb[c] = paletteColor.rgb[c];
            }
        }
    }
    
    return dImage;
}

void FloydSteinbergDitherer::ditherImageInPlaceWithPalette(const Image& image, const Palette& palette)
{
    Color pixelColor;
    Color paletteColor;
    for (int h = 0; h < image.getHeight(); h++)
    {
        for (int w = 0; w < image.getWidth(); w++)
        {
            Pixel* p = image.pixelAt(w, h);
            pixelColor.fromPixel(*p);
            palette.getClosestColorTo(pixelColor, paletteColor, p->palette_index);
            
            for (int c = 0; c < 3; c++)
            {
                float diff = pixelColor.rgb[c] - paletteColor.rgb[c];
                
                Pixel *pp;
                pp = image.pixelAt(w+1, h);
                if (pp) pp->rgb[c] += diff * 7.0 / 16.0;
                pp = image.pixelAt(w-1, h+1);
                if (pp) pp->rgb[c] += diff * 3.0 / 16.0;
                pp = image.pixelAt(w, h+1);
                if (pp) pp->rgb[c] += diff * 5.0 / 16.0;
                pp = image.pixelAt(w+1, h+1);
                if (pp) pp->rgb[c] += diff * 1.0 / 16.0;
                
                p->rgb[c] = paletteColor.rgb[c];
            }
        }
    }
}

class NearestNeighborDitherer : public Ditherer
{
public:
    Image* createDitheredImageFromImageWithPalette(const Image& image, const Palette& palette);
};

Image* NearestNeighborDitherer::createDitheredImageFromImageWithPalette(const Image &image, const Palette &palette)
{
    // create copy of image
    Image* dImage = new Image(image);
    
    Color pixelColor;
    Color paletteColor;
    //int pColorIndex = -1;
    for (int h = 0; h < dImage->getHeight(); h++)
    {
        for (int w = 0; w < dImage->getWidth(); w++)
        {
            Pixel* p = dImage->pixelAt(w, h);
            pixelColor.fromPixel(*p);
            palette.getClosestColorTo(pixelColor, paletteColor, p->palette_index);
            
            for (int c = 0; c < 3; c++)
            {
                p->rgb[c] = paletteColor.rgb[c];
            }
        }
    }
    
    return dImage;
}

class C64Ditherer : public Ditherer
{
public:
    Image* createDitheredImageFromImageWithPalette(const Image& image, const Palette& palette);
    static void threadFunc(C64Ditherer* ref, C64Image* image, int blockWidth, int blockHeight, int yBlock, int xBlocks, const Palette& palette);
    void processRow(C64Image* image, int blockWidth, int blockHeight, int yBlock, int xBlocks, const Palette& palette);
private:
    std::mutex process_mutex;
};

void C64Ditherer::threadFunc(C64Ditherer* ref, C64Image* image, int blockWidth, int blockHeight, int yBlock, int xBlocks, const Palette& palette)
{
    ref->processRow(image, blockWidth, blockHeight, yBlock, xBlocks, palette);
}

Image* C64Ditherer::createDitheredImageFromImageWithPalette(const Image &image, const Palette &palette)
{
    Ditherer* fsDitherer = createFloydSteinbergDitherer();
    int blockWidth = 4;
    int blockHeight = 8;
    
    int xBlocks = image.getWidth() / blockWidth;
    int yBlocks = image.getHeight() / blockHeight;
    
    C64Image* newImage = new C64Image(image);
    newImage->bgcolor = 0;
    newImage->fgcolor = 1;
    
    std::vector<std::thread> threads;
    for (int yb = 0; yb < yBlocks; yb++)
    {
        // create and run thread
        {
            // note: removed this mutex because it seemed unnecessary
            //std::lock_guard<std::mutex> guard(process_mutex);
            threads.push_back(std::thread(C64Ditherer::threadFunc, this, newImage, blockWidth, blockHeight, yb, xBlocks, palette));
        }
    }
    
    // join all threads
    for (auto& th : threads) th.join();
    
    delete fsDitherer;
    return newImage;
}

void C64Ditherer::processRow(C64Image* image, int blockWidth, int blockHeight, int yBlock, int xBlocks, const Palette& inpalette)
{
    Image SubImage(blockWidth, blockHeight);
    Ditherer* fsDitherer = createFloydSteinbergDitherer();
    Color avgColor;
    Color pColor;
    Color nextColor;
    Color pColor2;
    Palette p(4);
    
    int pcolors[4];
    pcolors[0] = 0;
    pcolors[1] = 1;
    
    for (int c = 0; c < 2; c++)
    {
        p.setColorAtIndex(*inpalette.colorAtIndex(pcolors[c]), c);
    }
    
    for (int xb = 0; xb < xBlocks; xb++)
    {
        Image* subImage = &SubImage;
        subImage->fromSubImage(*image, xb * blockWidth, blockWidth, yBlock * blockHeight, blockHeight);
        
        subImage->getAvgColor(avgColor);
        inpalette.getClosestColorTo(avgColor, pColor, pcolors[2], false);
        subImage->getSecondaryColor(pColor, nextColor, false);
        inpalette.getClosestColorTo(nextColor, pColor2, pcolors[3], false, pcolors[2]);
        
        for (int c = 2; c <= 3; c++)
        {
            p.setColorAtIndex(*inpalette.colorAtIndex(pcolors[c]), c);
        }
        fsDitherer->ditherImageInPlaceWithPalette(*subImage, p);
        
        for (int y = 0; y < subImage->getHeight(); y++)
        {
            for (int x = 0; x < subImage->getWidth(); x++)
            {
                Pixel* p = subImage->pixelAt(x, y);
                int c64_palette_color = pcolors[p->palette_index];
                p->palette_index = c64_palette_color;
            }
        }
        
        image->setBlockColor(xb, yBlock, 0, pcolors[2]);
        image->setBlockColor(xb, yBlock, 1, pcolors[3]);
        image->copyFromImageAtPosition(*subImage, xb*blockWidth, yBlock*blockHeight);
    }
     
    delete fsDitherer;
}

Ditherer* Ditherer::createFloydSteinbergDitherer()
{
    return new FloydSteinbergDitherer();
}

Ditherer* Ditherer::createC64Ditherer()
{
    return new C64Ditherer();
}

Ditherer* Ditherer::createNearestNeighborDitherer()
{
    return new NearestNeighborDitherer();
}
