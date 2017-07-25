//
//  C64Image.hpp
//  imconv
//
//  Created by Michael Hill on 3/15/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#ifndef C64Image_hpp
#define C64Image_hpp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Image.hpp"

class C64Image : public Image
{
public:
    C64Image(const Image& im)
    : Image(im)
    , xBlockSize(4)
    , yBlockSize(8)
    {
        xBlocks = getWidth() / xBlockSize;
        yBlocks = getHeight() / yBlockSize;
        blockColors = (int*)malloc(sizeof(int) * 2 * xBlocks * yBlocks);
    };
    
    C64Image(const Image& im, int w, int h)
    : Image(im, w, h)
    , xBlockSize(4)
    , yBlockSize(8)
    {
        xBlocks = getWidth() / xBlockSize;
        yBlocks = getHeight() / yBlockSize;
        blockColors = (int*)malloc(sizeof(int) * 2 * xBlocks * yBlocks);
    }
    
    ~C64Image()
    {
        if (blockColors)
        {
            free(blockColors);
            blockColors = NULL;
        }
    }
    
    int getXBlocks()
    {
        return xBlocks;
    }
    
    int getYBlocks()
    {
        return yBlocks;
    }
    
    void setBlockColor(int xb, int yb, int index, int color)
    {
        int bcindex = (yb * xBlocks * 2) + (xb * 2) + index;
        blockColors[bcindex] = color;
    }
    
    int getBlockColor(int xb, int yb, int index)
    {
        int bcindex = (yb * xBlocks * 2) + (xb * 2) + index;
        return blockColors[bcindex];
    }
    
    void getBitmapBytes(unsigned char* bitmap)
    {
        
    }
    
    int getC64FrameSize()
    {
        int c64FrameSize = sizeof(float) + 1024 + 8192;
        return c64FrameSize;
    }
    
    void getC64Frame(void *bytes, float pts)
    {
        memset(bytes, 0, getC64FrameSize());
        
        unsigned char* byteptr = (unsigned char*)bytes;
        float* timestamp = (float*)byteptr;
        printf("pts is %f\n", pts);
        *timestamp = pts;
        byteptr += sizeof(float);
        
        getColorBytes(byteptr);
        byteptr += 1024;
        getScreenBytes(byteptr);
    }
    
    int getColorBytesSize()
    {
        return xBlocks * yBlocks;
    }
    
    void getColorBytes(unsigned char* bytes)
    {
        // 1000 byte color map
        unsigned char* bptr = bytes;
        for (int yb = 0; yb < yBlocks; yb++)
        {
            for (int xb = 0; xb < xBlocks; xb++)
            {
                unsigned char c1 = getBlockColor(xb, yb, 0);
                unsigned char c2 = getBlockColor(xb, yb, 1);
                unsigned char colorbyte = (c1 << 4) | c2;
                *bptr = colorbyte;
                bptr++;
            }
        }
    }
    
    int getScreenBytesSize()
    {
        return xBlocks * yBlocks * 8;
    }
    
    void getScreenBytes(unsigned char* bytes)
    {
        int c64bytes = getScreenBytesSize();
        memset(bytes, 0, c64bytes);
        int width = getWidth();
        int height = getHeight();
        
        for (int h = 0; h < height; h++)
        {
            for (int w = 0; w < width; w++)
            {
                //printf("src %d = %d\n", h*width+w, src[h*width+w]);
                
                int xblock = w / xBlockSize;
                int yblock = h / yBlockSize;
                
                Pixel* p = pixelAt(w, h);
                
                // determine bitmask based on the palette index
                // bgcolor = 0x00
                // fgcolor = 0x03
                // block color 0 = 0x01
                // block color 1 = 0x02
                unsigned char mask = 0;
                
                if (p->palette_index == bgcolor)
                {
                    mask = 0x00;
                }
                else if (p->palette_index == fgcolor)
                {
                    mask = 0x03;
                }
                else if (p->palette_index == getBlockColor(xblock, yblock, 0))
                {
                    mask = 0x01;
                }
                else if (p->palette_index == getBlockColor(xblock, yblock, 1))
                {
                    mask = 0x02;
                }
                else
                {
                    printf("this should not happen.");
                }
                
                // set bits in bitmap byte
                int screen_width = w*2;
                
                int row = h / 8;
                int c = screen_width / 8;
                int line = h & 7;
                int bit = 7 - (screen_width & 7);
                int byte = row*320 + c*8 + line;
                
                unsigned char b = bytes[byte];
                
                // raise 2 bits (fg color)
                bit--;
                if (bit < 0)
                    bit = 0;
                
                unsigned char shifted_mask = mask << bit;
                b = b | shifted_mask;
                bytes[byte] = b;
            }
        }
    }
    
    int bgcolor;
    int fgcolor;
private:
    int xBlockSize;
    int yBlockSize;
    
    int xBlocks;
    int yBlocks;
    int* blockColors;
};

#endif /* C64Image_hpp */
