//
//  dct.cpp
//  petsciiConvert2
//
//  Created by Michael Hill on 9/18/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <math.h>
#define INVSQRT2 0.70710678118655

double alpha(double e)
{
    if (e == 0)
    {
        return INVSQRT2;
    }
    else 
    {
        return 1.0;
    }
}

void generateAlphaLookup(double **alphaLookup, int dim)
{
    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            alphaLookup[i][j] = alpha(i) * alpha(j);
        }
    }
}

void generateCosLookup(double ****cosLookup, double **alphaLookup, int height)
{
    double thisCos;
    for (int u = 0; u < height; u++)
    {
        for (int v = 0; v < height; v++)
        {
            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < height; j++)
                {
                    // generate cosine value
                    thisCos = cos(((M_PI*u)/(2*height))*(2*i + 1))*
                    cos(((M_PI*v)/(2*height))*(2*j + 1));
                    cosLookup[u][v][i][j] = thisCos * alphaLookup[i][j];
                }
            }
        }
    }
}

void generateCos1DLookup(double **cos1DLookup, int dim)
{
    double coeff = sqrt(2.0f / dim);
    for (int u = 0; u < dim; u++)
    {
        for (int i = 0; i < dim; i++)
        {
            cos1DLookup[u][i] = alpha(i) * cos( ((M_PI*u)/(2*dim)) * (2*i + 1) ) * coeff;
        }
    }
}

void dct1DWithInput(double* input, double* output, double** cos1DLookup, int dim)
{
    for (int u = 0; u < dim; u++)
    {
        double sum = 0;
        for (int i = 0; i < dim; i++)
        {
            sum += cos1DLookup[u][i] * input[i];
        }
        output[u] = sum;
    }
}

void dct1WithInput(double ** input, double *output, double **cos1DLookup, int dim)
{
    // do 2d dct by doing dim 1d dcts in either direction
    double* in = new double[dim];
    double* out = new double[dim];
    
    for (int x = 0; x < dim; x++)
    {
        // copy input
        for (int y = 0; y < dim; y++)
        {
            in[y] = input[x][y];
        }
        
        dct1DWithInput(in, out, cos1DLookup, dim);
        
        // copy output back
        for (int y = 0; y < dim; y++)
        {
            input[x][y] = out[y];
        }
    }
    
    for (int y = 0; y < dim; y++)
    {
        // copy input
        for (int x = 0; x < dim; x++)
        {
            in[x] = input[x][y];
        }
        
        dct1DWithInput(in, out, cos1DLookup, dim);
        
        // copy output back
        for (int x = 0; x < dim; x++)
        {
            //input[x][y] = out[x];
            output[x + y*dim] = out[x];
        }
    }
    
    delete[] in;
    delete[] out;
}

void dctWithInput(double ** input, double *output, double ****cosLookup, int dim)
{
    int u,v,i,j;
    double result;
    
    for(u = 0; u < dim; u++) // 
    {
        for(v = 0; v < dim; v++)
        {
            result = 0; // reset summed results to 0
            for(i = 0; i < dim; i++)
            {
                for(j = 0; j < dim; j++)
                {
                    result = result + (cosLookup[u][v][i][j]*input[i][j]);
                }
            }
            output[u+v*dim] = result / 4.0; //store the results
        }
    }
}
