//
//  Image.cpp
//  imconv
//
//  Created by Michael Hill on 2/20/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#include "Image.hpp"
#include <map>
#include <string>
#include <list>

using std::map;
using std::string;
using std::list;

Color::Color()
{
    for (int i = 0; i < 3; i++)
    {
        rgb[i] = 0.0;
    }
}

float Color::distanceFromColor(const Color &color)
{
    float error = 0;
    for (int i = 0; i < 3; i++)
    {
        float c = (color.rgb[i] - rgb[i]);
        error += (c*c);
    }
    
    return error;
}

void Color::fromPixel(const Pixel &pixel)
{
    for (int i = 0; i < 3; i++)
    {
        rgb[i] = pixel.rgb[i];
    }
}

void Color::fromColor(const Color &color)
{
    for (int i = 0; i < 3; i++)
    {
        rgb[i] = color.rgb[i];
    }
}

Palette::Palette()
: Palette(NULL, 0)
{
    
}

Palette::Palette(int nc)
: Palette(NULL, nc)
{
    
}

// Palette
Palette::Palette(unsigned char* colorValues, int nc)
: numColors(nc)
{
    // create colors from init values
    if (numColors > 0)
    {
        colors = new Color[numColors];
    }
    
    if (colorValues)
    {
        unsigned char* cc = colorValues;
        for (int i = 0; i < numColors; i++)
        {
            for (int c = 0; c < 3; c++)
            {
                colors[i].rgb[c] = ((float)(*cc) / 255.0);
                cc++;
            }
        }
    }
}

void Palette::setNumColors(int numColors)
{
    this->numColors = numColors;
    if (colors)
    {
        delete colors;
        colors = NULL;
    }
    
    colors = new Color[this->numColors];
}

void Palette::getClosestColorTo(const Color& inColor, Color& outColor, int& index) const
{
    float lowestError = 999999.9;
    int lowestErrorIndex = -1;
    
    for (int i = 0; i < numColors; i++)
    {
        Color* c = colorAtIndex(i);
        float error = c->distanceFromColor(inColor);
        if (error < lowestError)
        {
            lowestError = error;
            lowestErrorIndex = i;
        }
    }
    
    index = lowestErrorIndex;
    outColor.fromColor(*colorAtIndex(lowestErrorIndex));
}

Color* Palette::colorAtIndex(int index) const
{
    return &colors[index];
}

void Palette::setColorAtIndex(const Color &color, int index) const
{
    Color* c = colorAtIndex(index);
    c->fromColor(color);
}

Palette::~Palette()
{
    if (colors)
    {
        delete colors;
    }
}

// Pixel

Pixel::Pixel() {}

Image::Image(int w, int h, unsigned char* pixels)
: Image(w,h)
{
    unsigned char* pp = pixels;
    for (int hh = 0; hh < height; hh++)
    {
        for (int ww = 0; ww < width; ww++)
        {
            Pixel* p = pixelAt(ww, hh);
            for (int c = 0; c < 3; c++)
            {
                p->rgb[c] = (float)(*pp / 255.0);
                pp++;
            }
        }
    }
}

Image::Image(const Image& im)
: Image(im.getWidth(), im.getHeight())
{
    for (int hh = 0; hh < height; hh++)
    {
        for (int ww = 0; ww < width; ww++)
        {
            Pixel* p = pixelAt(ww, hh);
            Pixel* in_p = im.pixelAt(ww, hh);
            for (int c = 0; c < 3; c++)
            {
                p->rgb[c] = in_p->rgb[c];
            }
        }
    }
}

Image::Image(int w, int h)
: width(w)
, height(h)
{
    // allocate pixels
    pixels = new Pixel[width*height];
}

Image::~Image()
{
    if (pixels)
    {
        delete pixels;
        pixels = NULL;
    }
}

void Image::fromSubImage(const Image &im, int xOffset, int width, int yOffset, int height)
{
    // copy pixels
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Pixel* srcPixel = im.pixelAt(xOffset+x, yOffset+y);
            Pixel* dstPixel = pixelAt(x, y);
            
            for (int c = 0; c < 3; c++)
            {
                dstPixel->rgb[c] = srcPixel->rgb[c];
            }
        }
    }
}

void Image::getAvgColor(Color &color)
{
    color.set(0, 0, 0);
    float divisor = float(height*width);
    
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            Pixel* p = pixelAt(w, h);
            for (int c = 0; c < 3; c++)
            {
                color.rgb[c] += p->rgb[c] / divisor;
            }
        }
    }
}

void Image::colorHistogram()
{
    map<int, int> colorPixels;
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            Pixel* p = pixelAt(w, h);
            //printf("hh pixel %d %d %f %f %f\n", w, h, p->rgb[0], p->rgb[1], p->rgb[2]);
            /*
            sprintf(tmp, "%d_%d_%d__%d",
                    (int)(p->rgb[0]*255.0),
                    (int)(p->rgb[1]*255.0),
                    (int)(p->rgb[2]*255.0),
                    p->palette_index);
            
            string s(tmp);
            int n = colorPixels[s];
            n++;
            colorPixels[s] = n;
            */
            
            int n = colorPixels[p->palette_index];
            colorPixels[p->palette_index] = ++n;
        }
    }
    
    std::list<ColorRank> ranks;
    
    // sort
    while (colorPixels.size() > 0)
    {
        int maxVal = -1;
        int maxIndex = -1;
        for (std::map<int, int>::iterator it = colorPixels.begin(); it != colorPixels.end(); it++)
        {
            //printf("## %d: %d\n", it->first, it->second);
            if (it->second > maxVal)
            {
                maxVal = it->second;
                maxIndex = it->first;
            }
        }
        
        //printf("## %d: %d\n", maxIndex, maxVal);
        colorPixels.erase(maxIndex);
        
        ColorRank r;
        r.paletteIndex = maxIndex;
        r.numPixels = maxVal;
        
        ranks.push_back(r);
    }
    
    for (std::list<ColorRank>::iterator it = ranks.begin(); it != ranks.end(); it++)
    {
        printf("##@ %d: %d\n", it->paletteIndex, it->numPixels);
    }
    
    /*
    for (it = colorPixels.begin(); it != colorPixels.end(); it++)
    {
        printf("## %d: %d\n", it->first, it->second);
    }
    */
    
    
}

float Image::getErrorFromImage(const Image &im)
{
    float err = 0.0;
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            Pixel* p1 = pixelAt(w, h);
            Pixel* p2 = im.pixelAt(w, h);
            
            for (int c = 0; c < 3; c++)
            {
                err += p1->rgb[c] * p2->rgb[c];
            }
        }
    }
    return err;
}

Pixel* Image::pixelAt(int w, int h) const
{
    if (w >= 0 && w < width &&
        h >= 0 && h < height)
        return &pixels[h*width + w];
    else
        return NULL;
}

void Image::writePPM(const char *fname)
{
    FILE* fp = fopen(fname, "wb");
    
    // write ppm header
    fprintf(fp, "P6 %d %d 255\n", width, height);
    
    for (int hh = 0; hh < height; hh++)
    {
        for (int ww = 0; ww < width; ww++)
        {
            Pixel* p = pixelAt(ww, hh);
            for (int c = 0; c < 3; c++)
            {
                unsigned char clr = (unsigned char)(p->rgb[c] * 255.0);
                fwrite(&clr, 1, 1, fp);
            }
        }
    }
    fclose(fp);
}

