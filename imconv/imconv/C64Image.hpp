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
    
    /*
     void create_c64_bitmap(unsigned char *dest, unsigned char *src, int width, int height)
     {
     int c64bytes = (width/8) * (height/8) * 8;
     // clear
     memset(dest, 0, c64bytes);
     
     for (int h = 0; h < height; h++)
     {
     for (int w = 0; w < width; w++)
     {
     //printf("src %d = %d\n", h*width+w, src[h*width+w]);
     if (src[h*width + w] == 1)
     {
     int row = h/8;
     int c = w/8;
     int line = h & 7;
     int bit = 7 - (w & 7);
     int byte = row*320 + c*8 + line;
     
     //printf("h %d w %d: row %d col %d line %d bit %d byte %d\n",
     //       h,w,row,c,line,bit,byte);
     
     unsigned char b = dest[byte];
     unsigned char mask = 1 << bit;
     b = b | mask;
     dest[byte] = b;
     }
     }
     }
     }
    */
    
    /*
    void colormap_from_rgb(unsigned char *colormap, unsigned char *rgb, int width, int height, color *colors)
    {
        int rows = height/8;
        int columns = width/8;
        
        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < columns; c++)
            {
                int x = c*8;
                int y = r*8;
                int colormap_index = r*columns + c;
                
                unsigned char colorbyte = color_byte_for_block(rgb, x, y, width, height, colors);
                colormap[colormap_index] = colorbyte;
            }
        }
        //printf("got colormap\n");
    }
    */
    
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
                
                /*
                if (p->palette_index == 1)
                {
                    int screen_width = w*2;
                    //int screen_width = w;
                    
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
                    
                    unsigned char mask = 0x03 << bit;
                    b = b | mask;
                    bytes[byte] = b;
                }
                */
                
                /*
                if (src[h*width + w] == 1)
                {
                    int row = h/8;
                    int c = w/8;
                    int line = h & 7;
                    int bit = 7 - (w & 7);
                    int byte = row*320 + c*8 + line;
                    
                    //printf("h %d w %d: row %d col %d line %d bit %d byte %d\n",
                    //       h,w,row,c,line,bit,byte);
                    
                    unsigned char b = dest[byte];
                    unsigned char mask = 1 << bit;
                    b = b | mask;
                    dest[byte] = b;
                }
                */
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
