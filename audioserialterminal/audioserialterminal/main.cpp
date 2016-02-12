//
//  main.cpp
//  audioserialterminal
//
//  Created by Michael Hill on 1/27/16.
//  Copyright © 2016 jaunt. All rights reserved.
//

#include <iostream>
#include "AudioSerial.hpp"
#include "portaudio.h"

float abuffer[1024];

static int paTestCallback(const void *inputBuffer, void *outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void *userData )
{
    /*
    float *out = (float*)outputBuffer;
    unsigned long i;
    (void) timeInfo;
    (void) statusFlags;
    (void) inputBuffer;
    
    for (i = 0; i < framesPerBuffer; i++)
    {
        *out++ = 0.0;
        *out++ = 0.0;
    }
    */
    
    float *in = (float *)inputBuffer;
    
    // get only right channel
    for (int i = 0; i < framesPerBuffer; i++)
    {
        in++;
        abuffer[i] = *in;
        in++;
    }
    
    AudioSerialPort *port = (AudioSerialPort *)userData;
    port->readaudio(abuffer, (int)framesPerBuffer);
    
    return paContinue;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    AudioSerialPort port(48000.0, 19200.0);
    //unsigned char str[10000];
    //sprintf((char *)str, "once upon a time there was a cool dude. 12345678");
    
    /*
    // TEST
    FILE *fpin = fopen("input.txt", "rb");
    fseek(fpin, 0, SEEK_END);
    long len = ftell(fpin);
    fseek(fpin, 0, SEEK_SET);
    
    unsigned char *input = (unsigned char *)malloc(sizeof(unsigned char) * len);
    fread(input, sizeof(unsigned char), len, fpin);
    fclose(fpin);
    
    port.send(input, (int)len);
    
     
    // TEST
    
    FILE *fp = fopen("test.raw", "wb");
    float samples[1024];
    
    for (int i = 0; i < 100; i++)
    {
        port.getaudio(samples, 1024);
        port.readaudio(samples, 1024);
        fwrite(samples, sizeof(float), 1024, fp);
    }
    fclose(fp);
    */
    
    /*
    FILE *fpin = fopen("aa.raw", "rb");
    fseek(fpin, 0, SEEK_END);
    long len = ftell(fpin);
    fseek(fpin, 0, SEEK_SET);
    int numfloats = (int)len / sizeof(float);
    
    float *recv = (float *)malloc(sizeof(float) * numfloats);
    fread(recv, sizeof(float), numfloats, fpin);
    fclose(fpin);
    
    port.readaudio(recv, numfloats);
    */
    
    //PaStreamParameters outputParameters;
    PaStreamParameters inputParameters;
    PaStream *stream;
    
    PaError err = Pa_Initialize();
    if (err != paNoError)
    {
        printf("error!\n");
        exit(0);
    }
     
    
    /*
    inputParameters.device = Pa_GetDefaultInputDevice();
    if (inputParameters.device == paNoDevice)
    {
        fprintf(stderr,"Error: No default output device.\n");
        exit(0);
    }
    */
    
    /*
    PaDeviceIndex numDevices = Pa_GetDeviceCount();
    for (int i = 0; i < numDevices; i++)
    {
        const PaDeviceInfo *info = Pa_GetDeviceInfo(i);
        printf("device %d name: %s\n", i, info->name);
    }
    
    exit(0);
    */
    
    inputParameters.device = 1;
    if (inputParameters.device == paNoDevice)
    {
        fprintf(stderr,"Error: No default output device.\n");
        exit(0);
    }
    
    inputParameters.channelCount = 2;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowOutputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    
    err = Pa_OpenStream(&stream,
                        &inputParameters,
                        NULL,
                        48000,
                        512,
                        paClipOff,
                        paTestCallback,
                        &port);
    err = Pa_StartStream(stream);
    
    unsigned char temp[1024];
    for (int i = 0; i < 1000; i++)
    {
        int size = port.recv(temp, 1024);
        for (int i = 0; i < size; i++)
        {
            printf("%c", temp[i]);
        }
        Pa_Sleep(100);
    }
    
    err = Pa_CloseStream(stream);
    Pa_Terminate();
    
    printf("done.\n");
    return 0;
}
