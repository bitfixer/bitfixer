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
    
    Xmodem xmodem(sendport);
    xmodem.send((const unsigned char *)"SendThisWithXmodem", 18);
}

int terminal(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    //AudioSerialPort port(44100.0, 19200.0);
    
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
    
    /*
    FILE *fpin = fopen("out1.raw", "rb");
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
