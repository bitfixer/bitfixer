/*
 *  Image.h
 *  gamester
 *
 *  Created by Michael Hill on 8/23/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "png.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} pixel_t;

/* A picture. */

typedef struct  {
    pixel_t *pixels;
    size_t width;
    size_t height;
} bitmap_t;

void *loadPNGImage(char *imageName, int *imgheight, int *imgwidth);
int save_png_to_file (bitmap_t *bitmap, const char *path);
pixel_t * pixel_at (bitmap_t * bitmap, int x, int y);

#endif