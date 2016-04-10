//
//  DCT.cpp
//  soundcontroller
//
//  Created by Michael Hill on 4/9/16.
//  Copyright (c) 2016 Michael Hill. All rights reserved.
//

#include "DCT.h"
#include <math.h>

DCT::DCT(int len) :
length(len)
{
    float coeff = sqrtf((2.0 / (float)length));
    coslookup = (float **)malloc(sizeof(float *) * length);
    for (int i = 0; i < length; i++)
    {
        coslookup[i] = (float *)malloc(sizeof(float) * length);
        for (int u = 0; u < length; u++)
        {
            coslookup[i][u] = coeff * delta(i) * cosf( ((M_PI * u)/(2.0*length)) * (2*i + 1) );
        }
    }
}

DCT::~DCT()
{
    // dealloc lookup table
    
    for (int i = 0; i < length; i++)
    {
        free(coslookup[i]);
    }
    
    free(coslookup);
}

void DCT::getDCT(float *input, float *output, int size)
{
    // perform DCT
    for (int u = 0; u < size; u++)
    {
        float sum = 0;
        for (int i = 0; i < size; i++)
        {
            float res = coslookup[i][u] * input[i];
            sum += res;
        }
        
        output[u] = sum;
    }
}

float DCT::delta(int i)
{
    if (i == 0)
    {
        return 1.0/sqrt(2.0);
    }
    else
    {
        return 1.0;
    }
}