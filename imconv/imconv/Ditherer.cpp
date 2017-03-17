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
};

Image* C64Ditherer::createDitheredImageFromImageWithPalette(const Image &image, const Palette &palette)
{
    Ditherer* fsDitherer = createFloydSteinbergDitherer();
    int blockWidth = 4;
    int blockHeight = 8;
    
    int xBlocks = image.getWidth() / blockWidth;
    int yBlocks = image.getHeight() / blockHeight;
    
    printf("xblocks %d yblocks %d\n", xBlocks, yBlocks);
    
    Image subImage(blockWidth,blockHeight);
    Image diffImage(blockWidth,blockHeight);
    //Image* newImage = new Image(image);
    C64Image* newImage = new C64Image(image);
    Palette p(4);
    
    Color black;
    black.set(0, 0, 0);
    Color white;
    white.set(1, 1, 1);
    
    p.setColorAtIndex(black, 0);
    p.setColorAtIndex(white, 1);
    
    newImage->bgcolor = 0;
    newImage->fgcolor = 1;
    
    Color avgColor;
    Color nextColor;
    Color pColor;
    Color pColor2;
    Pixel diffPix;
    int pcolors[4];
    
    pcolors[0] = 0;
    pcolors[1] = 1;
    
    int* blockColors = (int*)calloc(sizeof(int), 2*yBlocks*xBlocks);
    
    for (int yb = 0; yb < yBlocks; yb++)
    {
        for (int xb = 0; xb < xBlocks; xb++)
        {
            subImage.fromSubImage(image, xb * blockWidth, blockWidth, yb * blockHeight, blockHeight);
            /*
            // METHOD 1 - full search
            float minError = 9999999.9;
            int minErrorIndex[2];
            for (int c1 = 2; c1 < palette.getNumColors(); c1++)
            {
                for (int c2 = c1+1; c2 < palette.getNumColors(); c2++)
                {
                    bool useColors = true;
                    Color* pc1 = palette.colorAtIndex(c1);
                    
                    if (pc1->rgb[0] == pc1->rgb[1] && pc1->rgb[0] == pc1->rgb[2])
                    {
                        useColors = false;
                    }
                    
                    p.setColorAtIndex(*pc1, 2);
                    Color* pc2 = palette.colorAtIndex(c2);
                    
                    if (pc2->rgb[0] == pc2->rgb[1] && pc2->rgb[0] == pc2->rgb[2])
                    {
                        useColors = false;
                    }
                    
                    p.setColorAtIndex(*pc2, 3);
                    
                    // TEST - disqualify grayscale
                    if (useColors)
                    {
                        Image* ii = fsDitherer->createDitheredImageFromImageWithPalette(subImage, p);
                        float err = ii->getErrorFromImage(subImage);
                        
                        if (err < minError)
                        {
                            minError = err;
                            minErrorIndex[0] = c1;
                            minErrorIndex[1] = c2;
                        }
                        
                        delete ii;
                    }
                }
            }
            //*/
            
            /*
            // Method 2 - find best color if that is the only color in the block
            // find first color
            int bestError = 99999999.9;
            int bestColor = -1;
            for (int c = 2; c < palette.getNumColors(); c++)
            {
                bool useColor = true;
                Color* pc1 = palette.colorAtIndex(c);
                if (pc1->rgb[0] == pc1->rgb[1] && pc1->rgb[0] == pc1->rgb[2])
                {
                    useColor = false;
                }
                
                if (useColor)
                {
                    p.setColorAtIndex(*pc1, 2);
                    p.setColorAtIndex(*pc1, 3);
                    
                    Image* ii = fsDitherer->createDitheredImageFromImageWithPalette(subImage, p);
                    float err = ii->getErrorFromImage(subImage);
                    
                    if (err < bestError)
                    {
                        bestError = err;
                        bestColor = c;
                    }
                    
                    delete ii;
                }
            }
            //*/
            
            // METHOD 3 - find first color by matching block's avg color
            subImage.getAvgColor(avgColor);
            int bestColor;
            palette.getClosestColorTo(avgColor, pColor, bestColor, false);
            
            //*
            // find second color
            float minError = 9999999.9;
            int minErrorIndex[2];
            int c1 = bestColor;
            Image** testImages;
            testImages = (Image**)calloc(sizeof(Image*), palette.getNumColors());
            
            for (int c2 = 2; c2 < palette.getNumColors(); c2++)
            {
                bool useColors = c1 != c2;
                if (useColors)
                {
                    Color* pc1 = palette.colorAtIndex(c1);
                    p.setColorAtIndex(*pc1, 2);
                    
                    Color* pc2 = palette.colorAtIndex(c2);
                    if (pc2->rgb[0] == pc2->rgb[1] && pc2->rgb[0] == pc2->rgb[2])
                    {
                        useColors = false;
                    }
                    
                    p.setColorAtIndex(*pc2, 3);
                }
                
                //printf("c2 %d\n", c2);
                // TEST - disqualify grayscale
                if (useColors)
                {
                    testImages[c2] = fsDitherer->createDitheredImageFromImageWithPalette(subImage, p);
                    float err = testImages[c2]->getErrorFromImage(subImage);
                    
                    if (err < minError)
                    {
                        minError = err;
                        minErrorIndex[0] = c1;
                        minErrorIndex[1] = c2;
                    }
                }
            }
            
            // remember color selections
            // TEMP
            //minErrorIndex[0] = 0;
            //minErrorIndex[1] = 0;
            
            //printf("c1 %d c2 %d\n", minErrorIndex[0], minErrorIndex[1]);
            
            newImage->setBlockColor(xb, yb, 0, minErrorIndex[0]);
            newImage->setBlockColor(xb, yb, 1, minErrorIndex[1]);
            
            Color* pc;
            pc = palette.colorAtIndex(minErrorIndex[0]);
            p.setColorAtIndex(*pc, 2);
            pc = palette.colorAtIndex(minErrorIndex[1]);
            p.setColorAtIndex(*pc, 3);
            
            pcolors[2] = minErrorIndex[0];
            pcolors[3] = minErrorIndex[1];
            
            Image* ii = testImages[minErrorIndex[1]];
            //Image* ii = fsDitherer->createDitheredImageFromImageWithPalette(subImage, p);
            
            // map to full c64 palette
            for (int y = 0; y < ii->getHeight(); y++)
            {
                for (int x = 0; x < ii->getWidth(); x++)
                {
                    Pixel* p = ii->pixelAt(x, y);
                    int c64_palette_color = pcolors[p->palette_index];
                    p->palette_index = c64_palette_color;
                }
            }
            
            // copy subimage into destination image
            newImage->copyFromImageAtPosition(*ii, xb*blockWidth, yb*blockHeight);
            
            for (int c = 0; c < palette.getNumColors(); c++)
            {
                if (testImages[c])
                {
                    delete testImages[c];
                }
            }
            free(testImages);
            
            // temp
            //free(ii);
        }
    }
     
    delete fsDitherer;
    return newImage;
}

#if 0
Image* C64Ditherer::createDitheredImageFromImageWithPalette(const Image &image, const Palette &palette)
{
    // determine FG/BG color for each 8x8 cell in the image
    int xBlocks = image.getWidth() / 8;
    int yBlocks = image.getHeight() / 8;
    
    Ditherer* ditherer = createNearestNeighborDitherer();
    
    Image subImage(8, 8);
    Color avgColor;
    Color bgColor;
    Color fgColor;
    int bgIndex;
    Palette* blockPalettes = new Palette[xBlocks*yBlocks];
    
    for (int yb = 0; yb < yBlocks; yb++)
    {
        for (int xb = 0; xb < xBlocks; xb++)
        {
            //*
            subImage.fromSubImage(image, xb * 8, 8, yb * 8, 8);
            Palette* p = &blockPalettes[yb*xBlocks + xb];
            p->setNumColors(2);
            
            Color* ff;
            float minError = 9999999.9;
            int minErrorIndex[2];
            
            /*
            for (int bgc = 0; bgc < palette.getNumColors(); bgc++)
            {
                ff = palette.colorAtIndex(bgc);
                p->setColorAtIndex(*ff, 0);
                for (int fgc = bgc+1; fgc < palette.getNumColors(); fgc++)
                {
                    ff = palette.colorAtIndex(fgc);
                    p->setColorAtIndex(*ff, 1);
                    
                    Image* nnImage = ditherer->createDitheredImageFromImageWithPalette(subImage, *p);
                    float err = nnImage->getErrorFromImage(subImage);
                    if (err < minError)
                    {
                        minError = err;
                        minErrorIndex[0] = bgc;
                        minErrorIndex[1] = fgc;
                    }
                    
                    delete nnImage;
                }
            }
            */
            
            minErrorIndex[0] = 0;
            minErrorIndex[1] = 1;
            
            for (int cc = 0; cc < 2; cc++)
            {
                ff = palette.colorAtIndex(minErrorIndex[cc]);
                p->setColorAtIndex(*ff, cc);
            }
            
            //printf("Block %d %d: colors %d %d\n", xb, yb, minErrorIndex[0], minErrorIndex[1]);
            //*/
            
            /*
            // copy subimage from original image
            subImage.fromSubImage(image, xb * 8, 8, yb * 8, 8);
            subImage.getAvgColor(avgColor);
            
            palette.getClosestColorTo(avgColor, bgColor, bgIndex);
            
            Palette* p = &blockPalettes[yb*xBlocks + xb];
            p->setNumColors(2);
            
            Color* ff = palette.colorAtIndex(bgIndex);
            p->setColorAtIndex(*ff, 0);
            
            float minError = 9999999.9;
            int minErrorIndex = -1;
            for (int fgc = 0; fgc < palette.getNumColors(); fgc++)
            {
                if (fgc != bgIndex)
                {
                    // set palette color
                    ff = palette.colorAtIndex(fgc);
                    p->setColorAtIndex(*ff, 1);
                    
                    Image* nnImage = ditherer->createDitheredImageFromImageWithPalette(subImage, *p);
                    float err = nnImage->getErrorFromImage(subImage);
                    if (err < minError)
                    {
                        minError = err;
                        minErrorIndex = fgc;
                    }
                    
                    delete nnImage;
                }
            }
            
            ff = palette.colorAtIndex(minErrorIndex);
            p->setColorAtIndex(*ff, 1);
            
            printf("Block %d %d: colors %d %d\n", xb, yb, bgIndex, minErrorIndex);
            //p->setColorAtIndex(*ff, 1);
            //*/
        }
    }
    
    Image* dImage = new Image(image);
    Color pixelColor;
    Color paletteColor;
    int pColorIndex = -1;
    for (int h = 0; h < dImage->getHeight(); h++)
    {
        int yBlockIndex = h / 8;
        for (int w = 0; w < dImage->getWidth(); w++)
        {
            Pixel* p = dImage->pixelAt(w, h);
            int xBlockIndex = w / 8;
            Palette* pal = &blockPalettes[yBlockIndex*xBlocks + xBlockIndex];
            
            pixelColor.fromPixel(*p);
            pal->getClosestColorTo(pixelColor, paletteColor, pColorIndex);
            
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
    
    delete ditherer;
    //delete blockPalettes;
    return dImage;
}
#endif

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
