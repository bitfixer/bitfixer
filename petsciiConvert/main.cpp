#include <stdio.h>
#include "Image.h"
#include "petsciiGlyphs.h"
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include "dct.h"
#include "timer.hpp"

#include <array>

double **dctInput;
double *dctOutput;
double **dctSignatures;
double **imagedat;
double ****cosLookup;
double **alphaLookup;
double *squareLookup;
double *sigSquareLookup;

int* glyphScoreLookup;
int* sortedGlyphDctIndices;
int num_files;


void init();
void convertImage(char *filename, int dim, float time = 0.0);
void convertImageFromRGB(unsigned char* rgb, int width, int height, int dim, float time, FILE* fp_out);
int getMatchingGlyph(double *dctSearch);

int main (int argc, char * const argv[]) {
    init();
    
    int c;
    float framerate = 10.0;
    int width = 320;
    int height = 200;
    FILE* fp_in = stdin;
    
    while ((c = getopt(argc, argv, "f:w:h:p:i:")) != -1)
    {
        if (c == 'f') // framerate
        {
            framerate = atof(optarg);
        }
        else if (c == 'w') // width
        {
            width = atoi(optarg);
        }
        else if (c == 'h') // height
        {
            height = atoi(optarg);
        }
        else if (c == 'p') // pixel format
        {
            
        }
        else if (c == 'i') // input fname
        {
            fp_in = fopen(optarg, "rb");
        }
    }
    
    int framesize = width * height * 3;
    unsigned char* frame = new unsigned char[framesize];
    float frameTime = 0.0;
    float frameInterval = 1.0 / framerate;
    while (fread(frame, 1, framesize, fp_in) == framesize)
    {
        convertImageFromRGB(frame, width, height, 8, frameTime, stdout);
        frameTime += frameInterval;
    }
    
    return 0;
}

double pixelBrightness(unsigned char *imageData, int width, int x, int y, int bytesPerPixel)
{
    int scale = 1;
    int pixelinfo = (((width*scale) * (y*scale)) + (x*scale)) * bytesPerPixel;
    
    unsigned char red = imageData[pixelinfo];
    unsigned char green = imageData[pixelinfo+1];
    unsigned char blue = imageData[pixelinfo+2];
    
    double dist;
    dist = sqrt((double)red*(double)red + (double)green*(double)green + (double)blue*(double)blue);
    dist = dist / sqrt(3.0);
    
    return dist;
}

void convertImageFromRGB(unsigned char* rgb, int width, int height, int dim, float time, FILE* fp_out)
{
    char bmpFname[100];
    bitmap_t outputPng;
    outputPng.width = width;
    outputPng.height = height;
    outputPng.pixels = (pixel_t *)calloc (sizeof (pixel_t), outputPng.width * outputPng.height);
    
    int matching;
    unsigned char glyphIndex;
    fprintf(stderr, "converting rgb frame at time %f\n", time);

    for (int y = 0; y < height; y += dim)
    {
        for (int x = 0; x < width; x += dim)
        {
            // copy values into input buffer
            //fprintf(stderr, "input:\n");
            for (int yy = 0; yy < dim; yy ++)
            {
                for (int xx = 0; xx < dim; xx++)
                {
                    dctInput[xx][yy] = pixelBrightness(rgb,width,x+xx,y+yy,3);
                    //fprintf(stderr, "%lf,", dctInput[xx][yy]);
                }
                //fprintf(stderr, "\n");
            }
            //fprintf(stderr, "\n");
            
            dctWithInput(dctInput, dctOutput, cosLookup, dim);
            
            /*
            fprintf(stderr, "output:\n");
            for (int yy = 0; yy < dim; yy ++)
            {
                for (int xx = 0; xx < dim; xx++)
                {
                    fprintf(stderr, "%lf,", dctOutput[yy+xx*dim]);
                }
                fprintf(stderr, "\n");
            }
            fprintf(stderr, "\n");
            */
             
            matching = getMatchingGlyph(dctOutput);
            glyphIndex = (unsigned char)matching;
            
            fwrite(&glyphIndex, 1, 1, fp_out);
            
            // write to png
            unsigned char *glyphString;
            int glyphPix = dim*dim;
            pixel_t *pixel;
            sprintf(bmpFname, "image_%0.4f.png", time);
            
            if (matching < 128)
            {
                glyphString = &glyphs[matching * glyphPix];
            }
            else
            {
                glyphString = &glyphs[(matching - 128)*glyphPix];
            }
            
            int ind;
            ind = 0;
            for (int yy = 0; yy < dim; yy ++)
            {
                for (int xx = 0; xx < dim; xx++)
                {
                    pixel = pixel_at(&outputPng, x+xx, y+yy);
                    
                    if ((glyphString[ind] == '0' && matching < 128) ||
                        (glyphString[ind] == '1' && matching >= 128))
                    {
                        pixel->red = 0;
                        pixel->green = 0;
                        pixel->blue = 0;
                    }
                    else
                    {
                        pixel->red = 255;
                        pixel->green = 255;
                        pixel->blue = 255;
                    }
                    
                    ind++;
                }
            }
        }
    }
    save_png_to_file(&outputPng, bmpFname);
}

void convertImage(char *filename, int dim, float time)
{
    unsigned char *imageData;
    int width,height;
    int x,y,xx,yy;
    int matching;
    int glyphPix;
    unsigned char glyphIndex;
    
    char pngFname[100];
    char bmpFname[100];
    char txtFname[100];
    unsigned char *glyphString;
    
    bitmap_t outputPng;
    pixel_t *pixel;
    
    sprintf(pngFname, "%s.png", filename);
    sprintf(bmpFname, "%s_out.png", filename);
    sprintf(txtFname, "%s.txt", filename);
    
    FILE *fp = fopen(txtFname, "wb");
    
    imageData = (unsigned char *)loadPNGImage(pngFname, &height, &width);
    
    outputPng.width = width;
    outputPng.height = height;
    outputPng.pixels = (pixel_t *)calloc (sizeof (pixel_t), outputPng.width * outputPng.height);
    
    glyphPix = dim*dim;
    
    printf("image: %d %d\n",height,width);
    
    Tools::Timer timer;
    double dctTime = 0.0;
    double matchTime = 0.0;
    double convTime = 0.0;
    
    for (y = 0; y < height; y += dim)
    {
        for (x = 0; x < width; x+= dim)
        {
            // copy values into input buffer
            for (yy = 0; yy < dim; yy ++)
            {
                for (xx = 0; xx < dim; xx++)
                {
                    timer.start();
                    dctInput[xx][yy] = pixelBrightness(imageData,width,x+xx,y+yy,4);
                    convTime += timer.getTime();
                }
            }
            
            timer.start();
            dctWithInput(dctInput, dctOutput, cosLookup, dim);
            dctTime += timer.getTime();
            
            timer.start();
            matching = getMatchingGlyph(dctOutput);
            matchTime += timer.getTime();
            
            glyphIndex = (unsigned char)matching;
            
            fwrite(&glyphIndex, 1, 1, fp);
             
            if (matching < 128)
            {
                glyphString = &glyphs[matching * glyphPix];
            }
            else 
            {
                glyphString = &glyphs[(matching - 128)*glyphPix];
            }
             
            int ind;
            ind = 0;
            for (yy = 0; yy < dim; yy ++)
            {
                for (xx = 0; xx < dim; xx++)
                {
                    pixel = pixel_at(&outputPng, x+xx, y+yy);
                    
                    if ((glyphString[ind] == '0' && matching < 128) ||
                        (glyphString[ind] == '1' && matching >= 128))
                    {
                        pixel->red = 0;
                        pixel->green = 0;
                        pixel->blue = 0;
                        
                    }
                    else 
                    {
                        pixel->red = 255;
                        pixel->green = 255;
                        pixel->blue = 255;
                    }
                    
                    ind++;

                }
            }
        }
    }
    save_png_to_file(&outputPng, bmpFname);
    
    unsigned char temp[256];
    memset(temp, 0, 256);
    sprintf((char*)temp, "%0.24f", time);
    fwrite(temp, 1, 24, fp);
    fclose(fp);
    
    fprintf(stderr, "dct time %lf match time %lf conversion time %lf\n", dctTime, matchTime, convTime);
}

double getDctDiffBetween(double *inputA, double *inputB, double sum_A, double sum_B, int dim)
{
    double score, diff;
    int points = dim*dim;
    score = 0;
    
    for (int i = 0; i < points; i++)
    {
        diff = inputA[i]*inputB[i];
        score += diff;
    }
    
    score = sum_A + sum_B - 2*score;
    return score;
}

int getMatchingGlyph(double *dctSearch)
{
    double lowest = -1;
    double curr_score;
    int matchIndex;
    
    double sumOfSquares;
    sumOfSquares = 0;
    
    for (int i = 0; i < 64; i++)
    {
        sumOfSquares += (dctSearch[i]*dctSearch[i]);
    }
    
    // get closest index into sorted glyph table
    int lookupIndex = (int)dctSearch[0];
    int sortedIndex = glyphScoreLookup[lookupIndex];
    
    int searchRange = 30;
    int startIndex = sortedIndex - searchRange;
    if (startIndex < 0)
    {
        startIndex = 0;
    }
    
    int endIndex = sortedIndex + searchRange;
    if (endIndex > 255)
    {
        endIndex = 255;
    }
    
    for (int d = startIndex; d < endIndex; d++)
    {
        int dctIndex = sortedGlyphDctIndices[d];
        curr_score = getDctDiffBetween(dctSearch, dctSignatures[dctIndex], sumOfSquares, sigSquareLookup[dctIndex], 8);
        
        if (lowest == -1)
        {
            matchIndex = dctIndex;
            lowest = curr_score;
        }
        else if (curr_score < lowest)
        {
            matchIndex = dctIndex;
            lowest = curr_score;
        }
    }
    
    return matchIndex;
}

int getMatchingGlyph2(double *dctSearch)
{
    double lowest = -1;
    double curr_score;
    int matchIndex;
    
    double sumOfSquares;
    sumOfSquares = 0;
    
    for (int i = 0; i < 64; i++)
    {
        sumOfSquares += (dctSearch[i]*dctSearch[i]);
    }
    
    for (int d = 0; d < 256; d++)
    {
        curr_score = getDctDiffBetween(dctSearch, dctSignatures[d], sumOfSquares, sigSquareLookup[d], 8);
        
        if (d == 0)
        {
            matchIndex = d;
            lowest = curr_score;
        }
        else if (curr_score < lowest)
        {
            matchIndex = d;
            lowest = curr_score;
        }
    }
    
    return matchIndex;
}


void prepareGlyphSignatures()
{
    unsigned char *glyphString;
    
    for (int ch = 0; ch < 256; ch++)
    {
        if (ch < 128)
        {
            glyphString = &glyphs[ch * 64];
        }
        else 
        {
            glyphString = &glyphs[(ch - 128) * 64];
        }
        
        int index = 0;
        unsigned char bit;
        for (int y = 0; y < 8; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                
                bit = glyphString[index];
                
                if (bit == '0')
                {
                    if (ch < 128)
                        dctInput[x][y] = 0;
                    else 
                        dctInput[x][y] = 255;
                }
                else 
                {
                    if (ch < 128)
                        dctInput[x][y] = 255;
                    else 
                        dctInput[x][y] = 0;
                }
                
                index++;
            }
        }
        
        dctWithInput(dctInput, dctSignatures[ch], cosLookup, 8);
        double thissig;
        thissig = 0;
        for (int i = 0; i < 64; i++)
        {
            thissig += (dctSignatures[ch][i] * dctSignatures[ch][i]);
        }
        sigSquareLookup[ch] = thissig;
    }
    
    // create sorted list of indices
    std::array<int, 256> indices;
    for (int i = 0; i < 256; i++)
    {
        indices[i] = i;
    }
    
    // sort function
    struct {
        bool operator()(int a, int b) const
        {
            return dctSignatures[a][0] < dctSignatures[b][0];
        }
    } compFn;
    std::sort(indices.begin(), indices.end(), compFn);
    
    sortedGlyphDctIndices = new int[256];
    
    for (int i = 0; i < 256; i++)
    {
        sortedGlyphDctIndices[i] = indices[i];
    }
    
    // create score lookup table
    glyphScoreLookup = new int[4096];
    for (int i = 0; i < 4096; i++)
    {
        double lookupScore = (double)i;
        int j = 0;
        for (j = 0; j < 256; j++)
        {
            if (dctSignatures[sortedGlyphDctIndices[j]][0] >= lookupScore)
            {
                break;
            }
        }
        
        if (j < 256)
        {
            glyphScoreLookup[i] = j;
        }
        else
        {
            glyphScoreLookup[i] = 255;
        }
    }
}

void init()
{
    imagedat = (double **)malloc(sizeof(double *) * 320.0);
    for (int i = 0; i < 320; i++)
    {
        imagedat[i] = (double *)malloc(sizeof(double) * 200.0);
    }
    
    dctSignatures = (double **)malloc(sizeof(double *) * 256);
    for (int i = 0; i < 256; i++)
    {
        dctSignatures[i] = (double *)malloc(sizeof(double) * 64);
    }
    
    dctInput = (double **)malloc(sizeof(double *) * 8);
    for (int i = 0; i < 8; i++)
    {
        dctInput[i] = (double *)malloc(sizeof(double) * 8);
    }
    
    dctOutput = (double *)malloc(sizeof(double) * 64);
    
    
    cosLookup = (double ****)malloc(sizeof(double ***) * 8);
    for (int i = 0; i < 8; i++)
    {
        cosLookup[i] = (double ***)malloc(sizeof(double**) * 8);
        
        for (int j = 0; j < 8; j++)
        {
            cosLookup[i][j] = (double **)malloc(sizeof(double *) * 8);
            
            for (int k = 0; k < 8; k++)
            {
                cosLookup[i][j][k] = (double *)malloc(sizeof(double) * 8);
            }
        }
    }
    
    alphaLookup = (double **)malloc(sizeof(double *) * 8);
    for (int i = 0; i < 8; i++)
    {
        alphaLookup[i] = (double *)malloc(sizeof(double) * 8);
    }
    
    sigSquareLookup = (double *)malloc(sizeof(double) * 256);
    generateAlphaLookup(alphaLookup, 8);
    generateCosLookup(cosLookup, alphaLookup, 8);
    prepareGlyphSignatures();
}
