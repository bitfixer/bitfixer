//
//  ViewController.m
//  iosserial
//
//  Created by Michael Hill on 2/14/16.
//  Copyright (c) 2016 Michael Hill. All rights reserved.
//

#import "ViewController.h"
#import <AVFoundation/AVFoundation.h>
#import <AudioUnit/AudioUnit.h>
#include <math.h>
#include "AudioSerial.hpp"

#define MAXINT 2147483647

@interface ViewController ()
@property (nonatomic, assign) float graphSampleRate;
@end

@implementation ViewController
{
    AudioSerialPort *port;
}


- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    port = new AudioSerialPort(48000.0, 19200.0);
    
    UITextView *textView = [[UITextView alloc] init];
    textView.frame = CGRectMake(0, 100, 200, 200);
    
    [self.view addSubview:textView];
    textView.text = @"Yo dawg";
    textView.backgroundColor = [UIColor redColor];
    textView.delegate = self;
    
    // configure audio session
    self.graphSampleRate = 48000;
    NSError *audioSessionError = nil;
    AVAudioSession *mySession = [AVAudioSession sharedInstance];
    [mySession setPreferredHardwareSampleRate:self.graphSampleRate error:&audioSessionError];
    [mySession setCategory:AVAudioSessionCategoryPlayAndRecord error:&audioSessionError];
    [mySession setActive:YES error:&audioSessionError];
    
    self.graphSampleRate = [mySession currentHardwareSampleRate];
    
    AudioComponentDescription desc;
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_RemoteIO;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;
    
    AUGraph processingGraph;
    NewAUGraph(&processingGraph);
    
    AUNode outNode;
    AUGraphAddNode(processingGraph, &desc, &outNode);
    
    AUGraphOpen(processingGraph);
    
    AudioUnit outUnit;
    AUGraphNodeInfo(processingGraph, outNode, NULL, &outUnit);
    
    
    AudioStreamBasicDescription streamDesc;
    streamDesc.mSampleRate = 48000.0;
    streamDesc.mFormatID = kAudioFormatLinearPCM;
    streamDesc.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked;
    streamDesc.mFramesPerPacket = 1;
    streamDesc.mChannelsPerFrame = 2;
    streamDesc.mBitsPerChannel = 32;
    streamDesc.mBytesPerPacket = 8;
    streamDesc.mBytesPerFrame = 8;
    
    AudioUnitSetProperty(outUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &streamDesc, sizeof(streamDesc));
    
    AURenderCallbackStruct callbackStruct;
    callbackStruct.inputProc = &renderCallback;
    callbackStruct.inputProcRefCon = port;
    
    AudioUnitSetProperty(outUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &callbackStruct, sizeof(callbackStruct));
    
    OSStatus result = AUGraphInitialize(processingGraph);
    AUGraphStart(processingGraph);
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text
{
    NSLog(@"new text: %@", text);
    port->send((unsigned char *)[text cStringUsingEncoding:NSUTF8StringEncoding], (int)[text length]);
    return YES;
}

static float phase = 0.0;
static float readBuffer[10000];

static OSStatus renderCallback
                    (	void *							inRefCon,
                    AudioUnitRenderActionFlags *	ioActionFlags,
                    const AudioTimeStamp *			inTimeStamp,
                    UInt32							inBusNumber,
                    UInt32							inNumberFrames,
                    AudioBufferList * __nullable	ioData)
{
    AudioSerialPort *port = (AudioSerialPort *)inRefCon;
    AudioBuffer *buf = &ioData->mBuffers[0];
    
    float step = 2.0 * M_PI * 12000.0 / 48000.0;
    port->getaudio(readBuffer, inNumberFrames);
    float *floatData = (float *)buf->mData;
    
    for (int i = 0; i < inNumberFrames; i++)
    {
        *floatData++ = readBuffer[i];
        *floatData++ = sin(phase);
        phase += step;
    }
    
    phase = fmodf(phase, 2.0f * M_PI);
    
    /*
    if (ioData)
    {
        printf("%d buffer list\n", ioData->mNumberBuffers);
        
        AudioBuffer *buf = &ioData->mBuffers[0];
        printf("%d channels %d bytesize\n", buf->mNumberChannels, buf->mDataByteSize);
        
        //short *samples = (short *)buf->mData;
        float *samples = (float *)buf->mData;
        float step = 2.0 * M_PI * 440.0 / 48000.0;
        for (int i = 0; i < inNumberFrames; i++)
        {
            float f = sinf(phase);
            //short s = (short)(f * 32767.0);
            
            *samples++ = f;
            //*samples++ = s;
            //*samples++ = l;
            
            phase += step;
        }
        
        while (phase >= (2.0f * M_PI))
        {
            phase -= 2.0f * M_PI;
        }
         
    }
    */
    
    return 0;
}

@end
