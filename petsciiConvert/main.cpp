#include <stdio.h>
#include "Image.h"
#include "petsciiGlyphs.h"
#include <math.h>
#include <stdlib.h>
#include "dct.h"

double **dctInput;
double *dctOutput;
double **dctSignatures;
double **imagedat;
double ****cosLookup;
double **alphaLookup;
double *squareLookup;
double *sigSquareLookup;
int num_files;


void init();
void convertImage(char *filename, int dim, float time = 0.0);
int getMatchingGlyph(double *dctSearch);

int main (int argc, char * const argv[]) {
    //printf("here i am..\n");
    char fname[100];
    int i;
    init();
    
    printf("converting image %s..\n",argv[1]);
    
    if (argc > 2)
    {
        num_files = atoi(argv[2]);
        float framerate = 10.0;
        if (argc > 3)
        {
            framerate = atof(argv[3]);
        }
        
        printf("there are %d files in dir\n", num_files);
        
        float currtime = 0.0;
        for (i = 1; i < num_files+1; i++)
        {   
            sprintf(fname, "%s/%d",argv[1],i);
            printf("converting %s\n",fname);
            convertImage(fname, 8, currtime);
            currtime += (1.0 / framerate);
        }
    }
    else 
    {
        convertImage(argv[1], 8);
    }
    
    return 0;
}

double pixelBrightness(unsigned char *imageData, int width, int x, int y)
{
    int scale = 1;
    int pixelinfo = (((width*scale) * (y*scale)) + (x*scale)) * 4;
    
    unsigned char red = imageData[pixelinfo];
    unsigned char green = imageData[pixelinfo+1];
    unsigned char blue = imageData[pixelinfo+2];
    
    double dist;
    dist = sqrt((double)red*(double)red + (double)green*(double)green + (double)blue*(double)blue);
    dist = dist / sqrt(3.0);
    
    return dist;
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
    
    for (y = 0; y < height; y += dim)
    {
        for (x = 0; x < width; x+= dim)
        {
            // copy values into input buffer
            for (yy = 0; yy < dim; yy ++)
            {
                for (xx = 0; xx < dim; xx++)
                {
                    dctInput[xx][yy] = pixelBrightness(imageData,width,x+xx,y+yy);
                }
            }
            
            dctWithInput(dctInput, dctOutput, cosLookup, dim);
            matching = getMatchingGlyph(dctOutput);
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
    //save_png_to_file(&outputPng, bmpFname);
    
    unsigned char temp[256];
    memset(temp, 0, 256);
    sprintf((char*)temp, "%0.24f", time);
    fwrite(temp, 1, 24, fp);
    
    /*
    fwrite(&time, sizeof(float), 1, fp);
    int remsize = 24 - sizeof(float);
    unsigned char zero = 0;
    for (int i = 0; i < remsize; i++)
    {
        fwrite(&zero, 1, 1, fp);
    }
    */
    
    fclose(fp);
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
    printf("generate Alpha lookup\n");
    generateAlphaLookup(alphaLookup, 8);
    printf("generate cos lookup\n");
    generateCosLookup(cosLookup, alphaLookup, 8);
    printf("prepare glyph signatures\n");
    prepareGlyphSignatures();
}