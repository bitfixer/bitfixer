//
//  FrequencyDetector.cpp
//  soundcontroller
//
//  Created by Michael Hill on 4/10/16.
//  Copyright (c) 2016 Michael Hill. All rights reserved.
//

#include "FrequencyDetector.h"
#include "math.h"

FrequencyDetector::FrequencyDetector(float sr, float mf, int ns) :
samplerate(sr),
maxfreq(mf),
numsteps(ns)
{
    dct = new DCT(numsteps);
    buffer = (float *)malloc(sizeof(float) * numsteps);
    dctoutput = (float *)malloc(sizeof(float) * numsteps);
    freq_table = (float *)malloc(sizeof(float) * numsteps);
    
    // calculate effective sample rate
    effective_sample_rate = maxfreq * 2.0;
    
    float increment = maxfreq / (numsteps-1);
    for (int i = 0; i < numsteps; i++)
    {
        freq_table[i] = i * increment;
    }
}

FrequencyDetector::~FrequencyDetector()
{
    if (dct)
    {
        delete dct;
        dct = NULL;
    }
    
    if (buffer)
    {
        free(buffer);
        buffer = NULL;
    }
    
    if (dctoutput)
    {
        free(dctoutput);
        dctoutput = NULL;
    }
    
    if (freq_table)
    {
        free(freq_table);
        freq_table = NULL;
    }
}

void FrequencyDetector::detect(float *samples, int numsamples)
{
    // perform low pass filtering
    lowPassFrequency(samples, inputbuffer, numsamples, samplerate, maxfreq);
    
    float end_time = (numsamples / samplerate);
    float sample_time = remainder_time;
    while (sample_time < end_time)
    {
        if (samples_in_buffer == numsteps)
        {
            // perform DCT
            dct->getDCT(buffer, dctoutput, numsteps);
            samples_in_buffer = 0;
            has_result = true;
        }
        
        int input_sample_index = (int)floor(sample_time * samplerate);
        
        // use nearest neighbor for efficiency
        buffer[samples_in_buffer] = inputbuffer[input_sample_index];
        samples_in_buffer++;
        
        sample_time += (1.0 / effective_sample_rate);
    }
    
    // prepare for next iteration
    remainder_time = sample_time-end_time;
}

float FrequencyDetector::getTopFrequency()
{
    if (!has_result)
        return -1.0;
    
    float max = -9999.0;
    int ind = 0;
    for (int i = 0; i < numsteps; i++)
    {
        float val = dctoutput[i] > 0 ? dctoutput[i] : -dctoutput[i];
        if (val > max && val > 0.1)
        {
            max = val;
            ind = i;
        }
    }
    
    return freq_table[ind];
}