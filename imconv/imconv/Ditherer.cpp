//
//  Ditherer.cpp
//  imconv
//
//  Created by Michael Hill on 2/20/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#include "Ditherer.hpp"

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
    Ditherer* ditherer = createNearestNeighborDitherer();
    Image* dIm = ditherer->createDitheredImageFromImageWithPalette(image, palette);
    dIm->colorHistogram();
    
    
    
    return dIm;
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