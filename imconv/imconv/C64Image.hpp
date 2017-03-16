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
    {
        //xBlocks = getWidth() / 8;
        xBlocks = (getWidth()*2) / 8;
        yBlocks = getHeight() / 8;
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
    
    void getColorBytes(unsigned char* bytes)
    {
        
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
                
                Pixel* p = pixelAt(w, h);
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
    int xBlocks;
    int yBlocks;
    int* blockColors;
};

#endif /* C64Image_hpp */
