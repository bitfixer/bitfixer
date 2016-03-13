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
#include "xmodem.hpp"
#include <iostream>
#include <thread>
#include <unistd.h>

float abuffer[1024];
float obuffer[1024];

static int inputCallback(const void *inputBuffer, void *outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void *userData )
{
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

static int outputCallback(const void *inputBuffer, void *outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void *userData )
{
    //printf("output\n");
    float *out = (float *)outputBuffer;
    AudioSerialPort *port = (AudioSerialPort *)userData;
    
    port->getaudio(obuffer, (int)framesPerBuffer);
    for (int i = 0; i < framesPerBuffer; i++)
    {
        *out++ = obuffer[i];
        *out++ = obuffer[i];
    }
    
    return paContinue;
}

AudioSerialPort *recvport;
AudioSerialPort *sendport;

void testrecv()
{
    Xmodem xmodem(recvport);
    xmodem.recv(NULL);
}

void testsend()
{
    sleep(1);
    //sendport->send((unsigned char *)"sending", 7);
    
    FILE *fp = fopen("input.txt", "rb");
    fseek(fp, 0, SEEK_END);
    long flen = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    unsigned char *buf = (unsigned char *)malloc(sizeof(unsigned char) * flen);
    fread(buf, sizeof(unsigned char), flen, fp);
    fclose(fp);
    
    Xmodem xmodem(sendport);
    xmodem.send(buf, (int)flen);
    
    free(buf);
}

int terminal(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    AudioSerialPort port(44100.0, 19200.0);
    
    /*
    sendport = new AudioSerialPort(44100.0, 19200.0);
    recvport = new AudioSerialPort(44100.0, 19200.0);
    
    std::thread recvthread(testrecv);
    std::thread sendthread(testsend);
    
    float sbuffer[1024];
    float rbuffer[1024];
    
    FILE *rfile = fopen("recv.raw", "wb");
    
    while(1)
    {
        sendport->getaudio(sbuffer, 1024);
        recvport->getaudio(rbuffer, 1024);
        
        sendport->readaudio(rbuffer, 1024);
        recvport->readaudio(sbuffer, 1024);
        
        fwrite(sbuffer, sizeof(float), 1024, rfile);
        usleep(23220);
    }
    */
    
    /*
    // TEST: write sequence
    AudioSerialPort port(44100.0, 19200.0);
    
    unsigned char temp[128];
    for (int i = 0; i < 128; i++)
    {
        temp[i] = i;
    }
    
    port.send(temp, 128);
    float buf[1024];
    FILE *fp = fopen("out1.raw", "wb");
    for (int i = 0; i < 44; i++)
    {
        port.getaudio(buf, 1024);
        fwrite(buf, sizeof(float), 1024, fp);
    }
    fclose(fp);
    */
    
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
    
    // TEST TEST
    /*
    FILE *text = fopen("2600-9-3.txt", "rb");
    unsigned char temp[128];
    fread(temp, 1, 128, text);
    fclose(text);
    
    for (int i = 0; i < 128; i++)
    {
        printf("%d: %02X\n", i, temp[i]);
    }
    
    uint16_t cksum = Xmodem::calcrc(temp, 128);
    printf("cksum is %d\n", cksum);
    */
    
    /*
    FILE *fpin = fopen("2600-9.3.txt", "rb");
    fseek(fpin, 0, SEEK_END);
    long len = ftell(fpin);
    fseek(fpin, 0, SEEK_SET);
    
    unsigned char temp;
    for (long i = 0; i < len; i++)
    {
        fread(&temp, 1, 1, fpin);
        
    }
    */
    
    /*
    FILE *fpin = fopen("2600_lopass.raw", "rb");
    fseek(fpin, 0, SEEK_END);
    long len = ftell(fpin);
    fseek(fpin, 0, SEEK_SET);
    int numfloats = (int)len / sizeof(float);
    
    
    FILE *fpout = fopen("2600_max.raw", "wb");
    FILE *fpmin = fopen("2600_min.raw", "wb");
    FILE *fpcenter = fopen("2600_center.raw", "wb");
    FILE *fpcorr = fopen("2600_corr.raw", "wb");
    
    float max_sample = 0.0;
    float min_sample = 0.0;
    float decay = 1.0 / 44.0;
    
    float prev_max_sample = 0.0;
    float prev_min_sample = 0.0;
    int prev_peak = 0;
    int prev_min = 0;
    int samplesWritten = 0;
    
    float *maxenvelope = (float *)calloc(sizeof(float),numfloats);
    float *minenvelope = (float *)calloc(sizeof(float),numfloats);
    
    float *maxptr = maxenvelope;
    float *minptr = minenvelope;
    
    for (int i = 0; i < numfloats; i++)
    {
        float sample;
        fread(&sample, sizeof(float), 1, fpin);
        
        if (sample > max_sample)
        {
            // set sample values on envelope
            int rampsamples = (i - prev_peak);
            float ramprise = sample - prev_max_sample;
            for (int s = 0; s < rampsamples; s++)
            {
                float portion = (float)s / (float)rampsamples;
                float thissample = prev_max_sample + (portion * ramprise);
                fwrite(&thissample, sizeof(float), 1, fpout);
                *maxptr = thissample;
                maxptr++;
                samplesWritten++;
            }
            max_sample = sample;
            prev_max_sample = max_sample;
            prev_peak = i;
        }
        else if (max_sample > decay)
        {
            max_sample -= decay;
        }
        
        if (sample < min_sample)
        {
            // set sample values on envelope
            int rampsamples = (i - prev_min);
            float ramprise = sample - prev_min_sample;
            for (int s = 0; s < rampsamples; s++)
            {
                float portion = (float)s / (float)rampsamples;
                float thissample = prev_min_sample + (portion * ramprise);
                fwrite(&thissample, sizeof(float), 1, fpmin);
                *minptr = thissample;
                minptr++;
            }
            min_sample = sample;
            prev_min_sample = min_sample;
            prev_min = i;
        }
        else if (min_sample < -decay)
        {
            min_sample += decay;
        }
        
        //fwrite(&max_sample, sizeof(float), 1, fpout);
    }
    
    fseek(fpin, 0, SEEK_SET);
    
    for (int i = 0; i < numfloats; i++)
    {
        float mid = (maxenvelope[i] + minenvelope[i]) / 2.0;
        fwrite(&mid, sizeof(float), 1, fpcenter);
        float s;
        fread(&s, sizeof(float), 1, fpin);
        float corr = s - mid;
        
        if (corr > 1.0)
            corr = 1.0;
        if (corr < -1.0)
            corr = -1.0;
        
        fwrite(&corr, sizeof(float), 1, fpcorr);
    }
    
    fclose(fpout);
    fclose(fpin);
    fclose(fpmin);
    fclose(fpcenter);
    fclose(fpcorr);
    
    printf("written %d read %d\n", samplesWritten, numfloats);
    */
     
    {
        FILE *fpin = fopen("audio_2600_2.raw", "rb");
        fseek(fpin, 0, SEEK_END);
        long len = ftell(fpin);
        fseek(fpin, 0, SEEK_SET);
        int numfloats = (int)len / sizeof(float);
        
        float *recv = (float *)malloc(sizeof(float) * numfloats);
        fread(recv, sizeof(float), numfloats, fpin);
        fclose(fpin);
        
        port.readaudio(recv, numfloats);
        Xmodem xm(&port);
        xm.recv(NULL);
    }
    
    
    //PaStreamParameters outputParameters;
    
    /*
    PaStream *stream;
    
    PaError err = Pa_Initialize();
    if (err != paNoError)
    {
        printf("error!\n");
        exit(0);
    }
    */
     
    
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
    
    /*
    PaStreamParameters inputParameters;
    // create input device
    inputParameters.device = 1;
    if (inputParameters.device == paNoDevice)
    {
        fprintf(stderr,"Error: No default output device.\n");
        exit(0);
    }
    
    inputParameters.channelCount = 2;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    
    err = Pa_OpenStream(&stream,
                        &inputParameters,
                        NULL,
                        48000,
                        512,
                        paClipOff,
                        inputCallback,
                        &port);
    err = Pa_StartStream(stream);
    
    PaStreamParameters outputParameters;
    PaStream *outputStream;
    // create output parameters
    outputParameters.device = Pa_GetDefaultOutputDevice();
    if (outputParameters.device == paNoDevice)
    {
        fprintf(stderr,"Error: No default output device.\n");
        exit(0);
    }
    
    outputParameters.channelCount = 2;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    
    err = Pa_OpenStream(&outputStream,
                        NULL,
                        &outputParameters,
                        48000,
                        512,
                        paClipOff,
                        outputCallback,
                        &port);
    err = Pa_StartStream(outputStream);
    
    std::string line = " ";
    for (int i = 0; i < 1000; i++)
    {
        std::cin >> line;
        if (line.length() > 0)
        {
            printf("got: %s %d chars\n", line.c_str(), (int)line.length());
            port.send((unsigned char *)line.c_str(), (int)line.length());
        }
        //Pa_Sleep(100);
    }
    
    err = Pa_CloseStream(stream);
    Pa_Terminate();
    
    printf("done.\n");
    */
    return 0;
}

int main(int argc, const char * argv[]) {
    
    /*
     std::string lineInput = " ";
     while (lineInput.length() > 0)
     {
     std::cin >> lineInput;
     std::cout << lineInput;
     }
     
     return 0;
     */
    
    return terminal(argc, argv);
}
