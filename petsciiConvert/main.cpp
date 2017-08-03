#include <stdio.h>
#include "petsciiGlyphs.h"
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "dct.h"
#include "timer.hpp"
#include "Image.hpp"

#include <array>
#include <algorithm>

#define USE_1D_DCT 1

double **dctInput;
double *dctOutput;
double **dctSignatures;
double ****cosLookup;
double **cos1DLookup;
double **alphaLookup;
double *squareLookup;
double *sigSquareLookup;

int* glyphScoreLookup;
int* sortedGlyphDctIndices;
int num_files;


void init();
void convertImage(char *filename, int dim, float time = 0.0);
void convertImageFromRGB(unsigned char* rgb, int width, int height, int dim, float time, FILE* fp_out, bool output_image);
void convertImageFromGray(unsigned char* gray,
                          int width,
                          int height,
                          int dim,
                          float time,
                          FILE* fp_out,
                          bool output_image);
int getMatchingGlyph(double *dctSearch);

typedef enum {
    RGB,
    GRAY
} pixelFormat;

int main (int argc, char * const argv[]) {
    init();
    
    int c;
    float framerate = 10.0;
    int width = 320;
    int height = 200;
    FILE* fp_in = stdin;
    pixelFormat pf = RGB;
    bool output_image = false;
    
    while ((c = getopt(argc, argv, "f:w:h:p:i:o")) != -1)
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
            if (strcmp(optarg, "rgb") == 0)
            {
                pf = RGB;
            }
            else if (strcmp(optarg, "gray") == 0)
            {
                pf = GRAY;
            }
        }
        else if (c == 'i') // input fname
        {
            fp_in = fopen(optarg, "rb");
        }
        else if (c == 'o') // output image flag
        {
            output_image = true;
        }
    }
    
    int framesize = (pf == RGB) ? width * height * 3 : width * height;
    unsigned char* frame = new unsigned char[framesize];
    float frameTime = 0.0;
    float frameInterval = 1.0 / framerate;
    while (fread(frame, 1, framesize, fp_in) == framesize)
    {
        if (pf == RGB)
        {
            convertImageFromRGB(frame, width, height, 8, frameTime, stdout, output_image);
        }
        else if (pf == GRAY)
        {
            convertImageFromGray(frame, width, height, 8, frameTime, stdout, output_image);
        }
        frameTime += frameInterval;
    }
    
    if (fp_in != stdin)
    {
        fclose(fp_in);
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

void convertImageFromRGB(unsigned char* rgb,
                         int width,
                         int height,
                         int dim,
                         float time,
                         FILE* fp_out,
                         bool output_image)
{
    unsigned char* gray = new unsigned char[width*height];
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            gray[y*width + x] = (unsigned char)pixelBrightness(rgb, width, x, y, 3);
        }
    }
    
    convertImageFromGray(gray, width, height, dim, time, fp_out, output_image);
    delete[] gray;
}


void convertImageFromGray(unsigned char* gray,
                          int width,
                          int height,
                          int dim,
                          float time,
                          FILE* fp_out,
                          bool output_image)
{
    char bmpFname[100];
    Image outputImage(width, height);
    Tools::Timer timer;
    double dctTime = 0.0;
    double matchTime = 0.0;
    double convTime = 0.0;
    
    int matching;
    unsigned char glyphIndex;
    fprintf(stderr, "converting rgb frame at time %f\n", time);
    sprintf(bmpFname, "image_%0.4f.ppm", time);

    for (int y = 0; y < height; y += dim)
    {
        for (int x = 0; x < width; x += dim)
        {
            // copy values into input buffer
            for (int yy = 0; yy < dim; yy ++)
            {
                for (int xx = 0; xx < dim; xx++)
                {
                    dctInput[xx][yy] = gray[(y+yy)*width + (x+xx)];
                }
            }
            
            timer.start();
#ifdef USE_1D_DCT
            dct1WithInput(dctInput, dctOutput, cos1DLookup, dim);
#else
            dctWithInput(dctInput, dctOutput, cosLookup, dim);
#endif
            dctTime += timer.getTime();
            
            timer.start();
            matching = getMatchingGlyph(dctOutput);
            matchTime += timer.getTime();
            
            glyphIndex = (unsigned char)matching;
            
            fwrite(&glyphIndex, 1, 1, fp_out);
            
            if (output_image)
            {
                // write to png
                unsigned char *glyphString;
                int glyphPix = dim*dim;
                
                
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
                        Pixel* pixel = outputImage.pixelAt(x+xx, y+yy);
                        
                        if ((glyphString[ind] == '0' && matching < 128) ||
                            (glyphString[ind] == '1' && matching >= 128))
                        {
                            pixel->rgb[0] = 0;
                            pixel->rgb[1] = 0;
                            pixel->rgb[2] = 0;
                        }
                        else
                        {
                            pixel->rgb[0] = 1.0;
                            pixel->rgb[1] = 1.0;
                            pixel->rgb[2] = 1.0;
                        }
                        
                        ind++;
                    }
                }
            }
        }
    }
    if (output_image)
    {
        outputImage.writePPM(bmpFname);
    }
    
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
        
#ifdef USE_1D_DCT
        dct1WithInput(dctInput, dctSignatures[ch], cos1DLookup, 8);
#else
        dctWithInput(dctInput, dctSignatures[ch], cosLookup, 8);
#endif
        
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
    
    cos1DLookup = (double**)malloc(sizeof(double*) * 8);
    for (int i = 0; i < 8; i++)
    {
        cos1DLookup[i] = (double*)malloc(sizeof(double) * 8);
    }
    
    alphaLookup = (double **)malloc(sizeof(double *) * 8);
    for (int i = 0; i < 8; i++)
    {
        alphaLookup[i] = (double *)malloc(sizeof(double) * 8);
    }
    
    sigSquareLookup = (double *)malloc(sizeof(double) * 256);
    generateAlphaLookup(alphaLookup, 8);
    generateCosLookup(cosLookup, alphaLookup, 8);
    generateCos1DLookup(cos1DLookup, 8);
    prepareGlyphSignatures();
}
