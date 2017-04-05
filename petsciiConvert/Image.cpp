/*
 *  Image.cpp
 *  gamester
 *
 *  Created by Michael Hill on 8/23/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Image.h"
#include <stdio.h>
#include <stdlib.h>
#include "Utils.h"
#include "png.h"

class PngImage
{
public:
	
	PngImage() {}
	
	~PngImage()
	{
		int i;
		for (i = 0; i < height; i++)
		{
			free(row_pointers[i]);
		}
	}
	
	int width;
	int height;
	png_byte color_type;
	png_byte bit_depth;
	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;
	png_bytep * row_pointers;
	
	void read_png_file(char *file_name);
	
};

void abort_(const char * s, ...)
{
	fprintf(stderr, "\n");
	abort();
}

void print_hex(unsigned char *data, int datalen)
{
	printf("reading %d bytes\n", datalen);
	for (int i = 0; i < datalen; i++)
	{
		printf("%02X ", data[i]);
	}
	printf("\n");
}

void PngImage::read_png_file(char* file_name)
{
	char header[8];	// 8 is the maximum size that can be checked
	int y;
	
	// open file and test for it being a png 
	FILE *fp = fopen(file_name, "rb");
	
	if (!fp)
		abort_("[read_png_file] File %s could not be opened for reading", file_name);
	fread(header, sizeof(char), 8, fp);
	if (png_sig_cmp((png_byte *)header, 0, 8))
		abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);
	
	// initialize stuff
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	if (!png_ptr)
		abort_("[read_png_file] png_create_read_struct failed");
	
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		abort_("[read_png_file] png_create_info_struct failed");
	
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[read_png_file] Error during init_io");
	
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);
	png_read_info(png_ptr, info_ptr);
	
	width = info_ptr->width;
	height = info_ptr->height;
	color_type = info_ptr->color_type;
	bit_depth = info_ptr->bit_depth;
	
	number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);
	
	// read file
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[read_png_file] Error during read_image");
	
	//printf("rowbytes: %d\n",info_ptr->rowbytes);
	
	row_pointers = (png_bytep*) my_malloc(sizeof(png_bytep) * height);
	for (y=0; y<height; y++)
		row_pointers[y] = (png_byte*) my_malloc(info_ptr->rowbytes);
	
	png_read_image(png_ptr, row_pointers);
	
	fclose(fp);
}



/* Given "bitmap", this returns the pixel of bitmap at the point 
 ("x", "y"). */

pixel_t * pixel_at (bitmap_t * bitmap, int x, int y)
{
    return bitmap->pixels + bitmap->width * y + x;
}

/* Write "bitmap" to a PNG file specified by "path"; returns 0 on
 success, non-zero on error. */

int save_png_to_file (bitmap_t *bitmap, const char *path)
{
    FILE * fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t x, y;
    png_byte ** row_pointers = NULL;
    /* "status" contains the return value of this function. At first
     it is set to a value which means 'failure'. When the routine
     has finished its work, it is set to a value which means
     'success'. */
    int status = -1;
    /* The following number is set by trial and error only. I cannot
     see where it it is documented in the libpng manual.
     */
    int pixel_size = 3;
    int depth = 8;
    
    fp = fopen (path, "wb");
    if (! fp) {
        goto fopen_failed;
    }
    
    png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        goto png_create_write_struct_failed;
    }
    
    info_ptr = png_create_info_struct (png_ptr);
    if (info_ptr == NULL) {
        goto png_create_info_struct_failed;
    }
    
    /* Set up error handling. */
    
    if (setjmp (png_jmpbuf (png_ptr))) {
        goto png_failure;
    }
    
    /* Set image attributes. */
    
    png_set_IHDR (png_ptr,
                  info_ptr,
                  bitmap->width,
                  bitmap->height,
                  depth,
                  PNG_COLOR_TYPE_RGB,
                  PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT,
                  PNG_FILTER_TYPE_DEFAULT);
    
    /* Initialize rows of PNG. */
    
    row_pointers = (png_byte **)png_malloc (png_ptr, bitmap->height * sizeof (png_byte *));
    for (y = 0; y < bitmap->height; ++y) {
        png_byte *row = 
        (png_byte *)png_malloc (png_ptr, sizeof (uint8_t) * bitmap->width * pixel_size);
        row_pointers[y] = row;
        for (x = 0; x < bitmap->width; ++x) {
            pixel_t * pixel = pixel_at (bitmap, x, y);
            *row++ = pixel->red;
            *row++ = pixel->green;
            *row++ = pixel->blue;
        }
    }
    
    /* Write the image data to "fp". */
    
    png_init_io (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    
    /* The routine has successfully written the file, so we set
     "status" to a value which indicates success. */
    
    status = 0;
    
    for (y = 0; y < bitmap->height; y++) {
        png_free (png_ptr, row_pointers[y]);
    }
    png_free (png_ptr, row_pointers);
    
png_failure:
png_create_info_struct_failed:
    png_destroy_write_struct (&png_ptr, &info_ptr);
png_create_write_struct_failed:
    fclose (fp);
fopen_failed:
    return status;
}


// load a PNG image and return a simple data array with the uncompressed image data
void *loadPNGImage(char *imageName, int *imgheight, int *imgwidth)
{	
	char header[8];
	PngImage thePng;
	thePng.read_png_file(imageName);
	
	//printf("width %d, height %d, color %d, bitdepth %d\n",thePng.width, thePng.height, thePng.color_type, thePng.bit_depth);
	//printf("rowbytes: %d\n",thePng.info_ptr->rowbytes);
	
	unsigned char *imageData = (unsigned char *)my_malloc( thePng.height * thePng.width * 4 );
	int h, w, b, index;
	int bytesperpixel = thePng.info_ptr->rowbytes / thePng.width;
	
	for (h = 0; h < thePng.height; h++)
	{
		for (w = 0; w < thePng.width; w++)
		{
			index = (h * (thePng.width * 4)) + (w * 4);
			for (b = 0; b < bytesperpixel; b++)
			{
				//imageData[index+b] = thePng.row_pointers[h][((thePng.width - w - 1)*4)+b];
				imageData[index+b] = thePng.row_pointers[h][(w*bytesperpixel)+b];
			}
			if (bytesperpixel < 4)
			{
				imageData[index+3] = 255;
			}
		}
	}
	
	*imgheight = thePng.height;
	*imgwidth = thePng.width;
	
	return (void *)imageData;
}