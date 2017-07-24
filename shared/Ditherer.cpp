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
    Image** subImages = NULL;
    Image** dImages = NULL;
    Image rowImage(blockWidth*xBlocks, blockHeight);
    Ditherer* fsDitherer = createFloydSteinbergDitherer();
    Color avgColor;
    Color pColor;
    Color nextColor;
    Color pColor2;
    Palette* palette = NULL;
    Palette p(4);
    
    Color black;
    black.set(0, 0, 0);
    Color white;
    white.set(1, 1, 1);
    
    p.setColorAtIndex(black, 0);
    p.setColorAtIndex(white, 1);
    int pcolors[4];
    pcolors[0] = 0;
    pcolors[1] = 1;
    
    int* blockColors = new int[2*xBlocks];
    dImages = new Image*[xBlocks];
    subImages = new Image*[xBlocks];
    for (int x = 0; x < xBlocks; x++)
    {
        subImages[x] = new Image(blockWidth, blockHeight);
    }
    
    {
        std::lock_guard<std::mutex> guard(process_mutex);
        palette = new Palette(inpalette);
        
        for (int xb = 0; xb < xBlocks; xb++)
        {
            subImages[xb]->fromSubImage(*image, xb * blockWidth, blockWidth, yBlock * blockHeight, blockHeight);
        }
    }
    
    Image** testImages;
    testImages = (Image**)calloc(sizeof(Image*), palette->getNumColors());
    
    for (int xb = 0; xb < xBlocks; xb++)
    {
        Image* subImage = subImages[xb];
        subImage->getAvgColor(avgColor);
        int bestColor;
        palette->getClosestColorTo(avgColor, pColor, bestColor, false);
        
        int minErrorIndex[2];
        int c1 = bestColor;
        
        subImage->getSecondaryColor(pColor, nextColor, false);
        int c2;
        palette->getClosestColorTo(nextColor, pColor2, c2, false, c1);
        minErrorIndex[0] = c1;
        minErrorIndex[1] = c2;
        Color* col1 = palette->colorAtIndex(c1);
        Color* col2 = palette->colorAtIndex(c2);
        p.setColorAtIndex(*col1, 2);
        p.setColorAtIndex(*col2, 3);
        
        Color* pc;
        pc = palette->colorAtIndex(minErrorIndex[0]);
        p.setColorAtIndex(*pc, 2);
        pc = palette->colorAtIndex(minErrorIndex[1]);
        p.setColorAtIndex(*pc, 3);
        
        pcolors[2] = minErrorIndex[0];
        pcolors[3] = minErrorIndex[1];
        
        blockColors[xb*2] = minErrorIndex[0];
        blockColors[xb*2 + 1] = minErrorIndex[1];
        
        dImages[xb] = fsDitherer->createDitheredImageFromImageWithPalette(*subImage, p);
        
        for (int y = 0; y < dImages[xb]->getHeight(); y++)
        {
            for (int x = 0; x < dImages[xb]->getWidth(); x++)
            {
                Pixel* p = dImages[xb]->pixelAt(x, y);
                int c64_palette_color = pcolors[p->palette_index];
                p->palette_index = c64_palette_color;
            }
        }
    }
    
    // copy subimage into destination image
    {
        std::lock_guard<std::mutex> guard(process_mutex);
        for (int xb = 0; xb < xBlocks; xb++)
        {
            Image* dimage = dImages[xb];
            image->setBlockColor(xb, yBlock, 0, blockColors[xb*2]);
            image->setBlockColor(xb, yBlock, 1, blockColors[xb*2 + 1]);
            image->copyFromImageAtPosition(*dimage, xb*blockWidth, yBlock*blockHeight);
        }
    }
     
    for (int c = 0; c < palette->getNumColors(); c++)
    {
        if (testImages[c])
        {
            delete testImages[c];
        }
    }
    free(testImages);
    
    for (int x = 0; x < xBlocks; x++)
    {
        delete dImages[x];
    }
    delete[] dImages;
    
    for (int x = 0; x < xBlocks; x++)
    {
        delete subImages[x];
    }
    delete[] subImages;
    
    delete[] blockColors;
    delete fsDitherer;
    delete palette;
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
