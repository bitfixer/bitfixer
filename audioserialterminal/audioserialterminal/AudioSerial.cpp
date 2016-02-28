//
//  AudioSerial.cpp
//  audioserialterminal
//
//  Created by Michael Hill on 1/27/16.
//  Copyright © 2016 jaunt. All rights reserved.
//

#include "AudioSerial.hpp"
#include "math.h"

void AudioSerialPort::send(unsigned char *data, int length)
{
    for (int i = 0; i < length; i++)
    {
        buffer->push(data[i]);
    }
}

int AudioSerialPort::recv(unsigned char *data, int length)
{
    int available = inputbuffer->getsize();
    if (available > length)
        available = length;
    
    for (int i = 0; i < available; i++)
    {
        data[i] = inputbuffer->pop();
    }
    
    return available;
}

void AudioSerialPort::getaudio(float *samples, int numsamples)
{
    // clear buffer
    memset(samples, 0, sizeof(float) * numsamples);
    
    int sampleIndex = 0;
    while (sampleIndex < numsamples)
    {
        if (current_write_state == STARTING)
        {
            if (burst_samples_remaining > 0)
            {
                //samples[sampleIndex] = curr_burst_sample;
                samples[sampleIndex] = 1.0;
                if (curr_burst_sample == 1.0)
                    curr_burst_sample = -1.0;
                else
                    curr_burst_sample = 1.0;
                
                sampleIndex++;
                burst_samples_remaining--;
            }
            else if (starting_samples_remaining > 0)
            {
                samples[sampleIndex] = 1.0;
                sampleIndex++;
                starting_samples_remaining--;
            }
            else
            {
                current_write_state = SENDING;
            }
        }
        else if (current_write_state == ENDING)
        {
            if (ending_samples_remaining > 0)
            {
                samples[sampleIndex] = 1.0;
                sampleIndex++;
                ending_samples_remaining--;
            }
            else
            {
                current_write_state = IDLE;
            }
        }
        else if (current_write_state == IDLE || current_write_state == SENDING)
        {
            if (bits_remaining > 0)
            {
                unsigned char current_bit_value = 0;
                int curr_bit = 10-bits_remaining;
                
                if (curr_bit == 0) // start bit
                {
                    current_bit_value = 0;
                }
                else if (curr_bit == 9) // stop bit
                {
                    current_bit_value = 1;
                }
                else
                {
                    int bitshift = curr_bit - 1;
                    current_bit_value = (curr_byte >> bitshift) & 0x01;
                }
                
                float current_sample_value = (current_bit_value == 0x01) ? 1.0 : -1.0;
                    
                if (samples_remaining_in_bit >= 1.0)
                {
                    samples[sampleIndex] = current_sample_value;
                    samples_remaining_in_bit -= 1.0;
                    // step to next sample
                    sampleIndex++;
                }
                else if (samples_remaining_in_bit > 0.0 && samples_remaining_in_bit < 1.0)
                {
                    samples[sampleIndex] += (samples_remaining_in_bit * current_sample_value);
                    samples_remaining_in_bit -= 1.0;
                    
                    // do not step to next sample, need the value from next sample
                    // step to next bit in this byte
                    bits_remaining--;
                }
                else if (samples_remaining_in_bit < 0.0)
                {
                    // add remainder
                    samples[sampleIndex] += (-samples_remaining_in_bit * current_sample_value);
                    samples_remaining_in_bit += samples_per_bit;
                    
                    // increment sample, stay on the same bit
                    sampleIndex++;
                }
                else if (samples_remaining_in_bit == 0.0)
                {
                    // step to next bit
                    bits_remaining--;
                    samples_remaining_in_bit = samples_per_bit;
                }
            }
            else
            {
                // get another byte from the buffer, if any exist
                if (buffer->getsize() > 0)
                {
                    curr_byte = buffer->pop();
                    //printf("sending: %c\n", curr_byte);
                    bits_remaining = 10; // 1 start, 8 data, 1 stop
                    
                    if (current_write_state == IDLE)
                    {
                        // starting a new transmission
                        current_write_state = STARTING;
                        // send about 1 byte's worth of 1 samples
                        starting_samples_remaining = (int)(samples_per_bit * 10);
                        burst_samples_remaining = starting_samples_remaining;
                    }
                }
                else
                {
                    // queue is empty
                    samples_remaining_in_bit = samples_per_bit;
                    
                    if (current_write_state == SENDING)
                    {
                        current_write_state = ENDING;
                        ending_samples_remaining = (int)(samples_per_bit * 10);
                        //sampleIndex++;
                    }
                    else
                    {
                        // don't write any more samples
                        sampleIndex = numsamples;
                        current_write_state = IDLE;
                    }
                }
            }
        }
    }
}

void AudioSerialPort::readaudio(float *samples, int numsamples)
{
    int samplesLeft = numsamples;
    float *buf = samples;
    while (samplesLeft > 0)
    {
        int samplesToProcess = samplesLeft;
        if (samplesToProcess > max_input_buffer_size)
        {
            samplesToProcess = max_input_buffer_size;
        }
        
        readAudioInternal(buf, samplesToProcess);
        buf += samplesToProcess;
        samplesLeft -= samplesToProcess;
    }
}

void AudioSerialPort::readAudioInternal(float *s_in, int numsamples_in)
{
    // read incoming audio and read serial bytes
    
    // do oversampling
    int numsamples = numsamples_in * oversampling;
    
    for (int s = 0; s < numsamples; s++)
    {
        float pos = (float)s / (float)oversampling;
        int leftIndex = (int)floor(pos);
        int rightIndex = leftIndex+1;
        
        float sample = 0.0;
        if (rightIndex >= numsamples_in)
        {
            sample = s_in[numsamples_in-1];
        }
        else
        {
            float portion = pos - (float)leftIndex;
            sample = (1.0 - portion) * s_in[leftIndex] + (portion) * s_in[rightIndex];
            oversampling_buffer[s] = sample;
        }
    }
    
    float *samples = oversampling_buffer;
    
    // states can be:
    // searching (not in the middle of a byte)
    // reading (reading a byte)
    int curr_sample = 0;
    while (curr_sample < numsamples)
    {
        if (current_state == SEARCHING || current_state == NEXT_START_BIT)
        {
            // look for the start of a byte
            // this will be a start bit (0)
            
            float threshold = 0.5;
            if (current_state == NEXT_START_BIT)
            {
                //threshold = 0.15;
                //threshold = -0.15;
                //threshold = 0.15;
                threshold = - (last_start_bit_search_sample / 1.5 );
            }
            
            if (samples[curr_sample] < -threshold)
            {
                /*
                printf("found at %d : %0.3f\n", curr_sample, (float)curr_sample / samplerate);
                
                for (int i = curr_sample-10; i < curr_sample+10; i++)
                {
                    printf("s[%d] = %f\n", i, samples[i]);
                }
                */
                
                
                curr_input_byte = 0;
                curr_input_bit = 0;
                curr_sample_in_input_byte = 0;
                curr_min_sample = 1.0;
                curr_max_sample = -1.0;
                current_state = READING;
                //printf("Byte: start sample %d\n", curr_sample);
                
                for (int i = 0; i < 10; i++)
                {
                    input_bit_buckets[i] = 0.0;
                    input_bit_count[i] = 0.0;
                }
            }
            else
            {
                if (current_state == NEXT_START_BIT)
                {
                    start_bit_search_samples++;
                    if (start_bit_search_samples > 10)
                    {
                        current_state = SEARCHING;
                    }
                }
                curr_sample++;
            }
        }
        else if (current_state == READING)
        {
            // decide which bucket to put this sample in
            int next_sample = curr_sample_in_input_byte + 1;
            
            double sstart = (double)curr_sample_in_input_byte / ovr_samples_per_bit;
            double send = (double)next_sample / ovr_samples_per_bit;
            
            int this_bucket = (int)floor(sstart);
            int next_bucket = (int)floor(send);
            
            if (samples[curr_sample] > curr_max_sample)
                curr_max_sample = samples[curr_sample];
            
            if (samples[curr_sample] < curr_min_sample)
                curr_min_sample = samples[curr_sample];
            
            if (this_bucket > 9) // look for stop bit
            {
                float midpoint = (curr_min_sample + curr_max_sample) / 2.0;
                
                float val_diff = curr_max_sample - curr_min_sample;
                
                float prev_avg = input_bit_buckets[0] / input_bit_count[0];
                int prev_bit_value = 0;
                
                float stop_bit_avg = input_bit_buckets[9] / input_bit_count[9];
                if (stop_bit_avg < midpoint)
                {
                    printf("stop bit is messed up\n");
                    
                }
                    
                // done searching
                // push result to input buffer
                for (int i = 1; i < 9; i++)
                {
                    curr_input_byte >>= 1;
                    float bucket_avg = input_bit_buckets[i] / input_bit_count[i];
                    
                    if (bucket_avg > midpoint)
                    {
                        curr_input_byte |= 0x80;
                    }
                    
                }
                
                float startavg = input_bit_buckets[0] / input_bit_count[0];
                //printf("%c : %02X at sample %d : %0.6f\n", curr_input_byte, curr_input_byte, curr_sample, (float)curr_sample / samplerate);
                printf("%c", curr_input_byte);
                //printf("%c : %02X start %f stop %f startavg %f\n", curr_input_byte, curr_input_byte, input_bit_buckets[0], input_bit_buckets[9], startavg);
                //inputbuffer->push(curr_input_byte);
                start_bit_search_samples = 0;
                last_start_bit_search_sample = samples[curr_sample];
                
                current_state = NEXT_START_BIT;
            }
            else if (this_bucket == next_bucket)
            {
                // this sample is fully within one bit
                input_bit_buckets[this_bucket] += samples[curr_sample];
                input_bit_count[this_bucket] += 1.0;
                curr_sample_in_input_byte++;
                curr_sample++;
            }
            else if (this_bucket < next_bucket)
            {
                // this sample spans two buckets
                double first_portion = (double)next_bucket - sstart;
                double second_portion = send - (double)next_bucket;
                
                double first_pct = first_portion / ovr_bits_per_sample;
                double second_pct = second_portion / ovr_bits_per_sample;
                
                input_bit_buckets[this_bucket] += first_pct * samples[curr_sample];
                input_bit_count[this_bucket] += first_pct;
                
                if (next_bucket < 10)
                {
                    input_bit_buckets[next_bucket] += second_pct * samples[curr_sample];
                    input_bit_count[this_bucket] += second_pct;
                }
                
                curr_sample_in_input_byte++;
                curr_sample++;
            }
            
            
        }
        
    }
    
    //free(oversampling_buffer);
}
