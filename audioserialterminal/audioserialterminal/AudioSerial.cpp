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
                samples[sampleIndex] = curr_burst_sample;
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
                samples[sampleIndex] = -1.0;
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
                        sampleIndex++;
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
    // read incoming audio and read serial bytes
    
    // states can be:
    // searching (not in the middle of a byte)
    // reading (reading a byte)
    float threshold = 0.15;
    int curr_sample = 0;
    while (curr_sample < numsamples)
    {
        if (current_state == SEARCHING)
        {
            // look for the start of a byte
            // this will be a start bit (0)
            
            if (samples[curr_sample] < -threshold)
            {
                curr_input_byte = 0;
                curr_input_bit = 0;
                curr_sample_in_input_byte = 0;
                current_state = READING;
                
                for (int i = 0; i < 10; i++)
                {
                    input_bit_buckets[i] = 0.0;
                    input_bit_count[i] = 0.0;
                }
            }
            else
            {
                curr_sample++;
            }
        }
        else if (current_state == READING)
        {
            // decide which bucket to put this sample in
            int next_sample = curr_sample_in_input_byte + 1;
            
            double sstart = (double)curr_sample_in_input_byte / samples_per_bit;
            double send = (double)next_sample / samples_per_bit;
            
            int this_bucket = (int)floor(sstart);
            int next_bucket = (int)floor(send);
            
            if (this_bucket >= 9) // look for stop bit
            {
                if (samples[curr_sample] > threshold)
                {
                    // get running values for 0/1 bit voltages
                    float min_avg = input_bit_buckets[0] / input_bit_count[0];
                    float max_avg = samples[curr_sample] / samples_per_bit;
                    
                    float midpoint = (min_avg + max_avg) / 2.0;
                    
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
                    printf("%c", curr_input_byte);
                    //inputbuffer->push(curr_input_byte);
                    current_state = SEARCHING;
                }
            }
            else if (this_bucket == next_bucket)
            {
                // this sample is fully within one bit
                input_bit_buckets[this_bucket] += samples[curr_sample];
                input_bit_count[this_bucket] += 1.0;
                curr_sample_in_input_byte++;
            }
            else if (this_bucket < next_bucket)
            {
                // this sample spans two buckets
                double first_portion = (double)next_bucket - sstart;
                double second_portion = send - (double)next_bucket;
                
                double first_pct = first_portion / bits_per_sample;
                double second_pct = second_portion / bits_per_sample;
                
                input_bit_buckets[this_bucket] += first_pct * samples[curr_sample];
                input_bit_count[this_bucket] += first_pct;
                
                input_bit_buckets[next_bucket] += second_pct * samples[curr_sample];
                input_bit_count[this_bucket] += second_pct;
                
                curr_sample_in_input_byte++;
            }
            
            curr_sample++;
        }
        
    }
}
