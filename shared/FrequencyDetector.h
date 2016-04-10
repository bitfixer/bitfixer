//
//  FrequencyDetector.h
//  soundcontroller
//
//  Created by Michael Hill on 4/10/16.
//  Copyright (c) 2016 Michael Hill. All rights reserved.
//

#ifndef __soundcontroller__FrequencyDetector__
#define __soundcontroller__FrequencyDetector__

#include "DCT.h"

class FrequencyDetector
{
public:
    FrequencyDetector(float samplerate, float maxfreq, int numsteps);
    ~FrequencyDetector();
    void detect(float *samples, int numsamples);
    float getTopFrequency();
    
private:
    
    void lowPassFrequency(float* input,
                          float* output,
                          int points,
                          float samplerate,
                          float cutoff)
    {
        double RC = 1.0/(cutoff*2*3.14);
        double dt = 1.0/samplerate;
        double alpha = dt/(RC+dt);
        output[0] = input[0];
        for(int i = 1; i < points; ++i)
        {
            output[i] = output[i-1] + (alpha*(input[i] - output[i-1]));
        }
    }
    
    float samplerate;
    float maxfreq;
    int numsteps;
    DCT *dct = NULL;
    float inputbuffer[2048];
    
    float *buffer = NULL;
    float *dctoutput = NULL;

    int samples_in_buffer = 0;
    
    float effective_sample_rate;
    
    float current_time = 0.0;
    float remainder_time = 0.0;
    
    float *freq_table;
    bool has_result = false;
};

#endif /* defined(__soundcontroller__FrequencyDetector__) */
