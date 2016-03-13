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
//#import "Novocaine.h"
#include <math.h>
#include "AudioSerial.hpp"
#include "Xmodem.hpp"
#include "Novocaine.h"

#define MAXINT 2147483647
#define kOutputBus 0
#define kInputBus 1

@interface ViewController ()
@property (nonatomic, assign) float graphSampleRate;
@end

@implementation ViewController
{
    AudioSerialPort *port;
    UITextView *textView;
    AudioComponentInstance audioUnit;
    AudioStreamBasicDescription inputFormat;
    Novocaine *audioManager;
    
    FILE *fp_debug;
    float temp[10000];
    float phase;
}

void checkStatus(OSStatus status)
{
    
}

+ (NSString *) applicationDocumentsDirectory
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *basePath = paths.firstObject;
    return basePath;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    //port = new AudioSerialPort(44100.0, 38400.0);
    port = new AudioSerialPort(44100.0, 19200.0);
    //port = new AudioSerialPort(44100.0, 14400.0);
    //port = new AudioSerialPort(44100.0, 9600.0);
    phase = 0.0;
    
    textView = [[UITextView alloc] init];
    [self.view addSubview:textView];
    
    textView.text = @"";
    textView.backgroundColor = [UIColor redColor];
    textView.delegate = self;
    
    
    UIButton *button = [[UIButton alloc] init];
    button.frame = CGRectMake(0, 0, 100, 100);
    button.backgroundColor = [UIColor greenColor];
    [self.view addSubview:button];
    
    [button addTarget:self action:@selector(receive) forControlEvents:UIControlEventTouchUpInside];
    
    // configure audio session
    self.graphSampleRate = 48000;
    
    NSString *docsDir = [ViewController applicationDocumentsDirectory];
    NSString *audioFile = [docsDir stringByAppendingPathComponent:@"audio_blarg.raw"];
    fp_debug = fopen([audioFile cStringUsingEncoding:NSUTF8StringEncoding], "wb");
    
    
    NSString *inputFile = [docsDir stringByAppendingPathComponent:@"input.txt"];
    FILE *fp = fopen([inputFile cStringUsingEncoding:NSUTF8StringEncoding], "rb");
    fseek(fp, 0, SEEK_END);
    long flen = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    unsigned char *buf = (unsigned char *)malloc(sizeof(unsigned char) * flen);
    fread(buf, sizeof(unsigned char), flen, fp);
    fclose(fp);
    
    audioManager = [Novocaine audioManager];
    __weak ViewController *wself = self;
    
    
    [audioManager setInputBlock:^(float *data, UInt32 numFrames, UInt32 numChannels) {
        ViewController *vc = (ViewController *)wself;
        vc->port->readaudio(data, numFrames);
        fwrite(data, sizeof(float), numFrames, vc->fp_debug);
    }];
    
    [audioManager setOutputBlock:^(float *data, UInt32 numFrames, UInt32 numChannels) {
        ViewController *vc = (ViewController *)wself;
        vc->port->getaudio(vc->temp, numFrames);
        
        float step = 2.0 * M_PI * 12000.0 / 44100.0;
        
        for (int i = 0; i < numFrames; i++)
        {
            *data++ = temp[i];
            *data++ = sin(vc->phase);
            //*data++ = 0.0;
            phase += step;
        }
        
        vc->phase = fmodf(vc->phase, 2.0f * M_PI);
        
        /*
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
         
         return 0;
        */
        
    }];
    
    [audioManager play];
    
    
    
    /*
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        Xmodem xmodem(port);
        //xmodem.send(buf, (int)flen);
        xmodem.recv(NULL);
    });
    */

    
    //[self performSelector:@selector(receive) withObject:nil afterDelay:5.0];
    
    
    
     
    /*
    NSError *audioSessionError = nil;
    AVAudioSession *mySession = [AVAudioSession sharedInstance];
    [mySession setPreferredHardwareSampleRate:self.graphSampleRate error:&audioSessionError];
    [mySession setCategory:AVAudioSessionCategoryPlayAndRecord error:&audioSessionError];
    [mySession setActive:YES error:&audioSessionError];
    self.graphSampleRate = [mySession currentHardwareSampleRate];
    
    AUGraph processingGraph;
    NewAUGraph(&processingGraph);
    
    AUNode ioNode;
    
    AudioComponentDescription desc;
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_RemoteIO;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;
    
    AUGraphAddNode(processingGraph, &desc, &ioNode);

    AUGraphOpen(processingGraph);
    
    AudioUnit outUnit;
    AUGraphNodeInfo(processingGraph, ioNode, NULL, &outUnit);
    
    AudioStreamBasicDescription streamDesc;
    streamDesc.mSampleRate = 48000.0;
    streamDesc.mFormatID = kAudioFormatLinearPCM;
    streamDesc.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked;
    streamDesc.mFramesPerPacket = 1;
    streamDesc.mChannelsPerFrame = 2;
    streamDesc.mBitsPerChannel = 32;
    streamDesc.mBytesPerPacket = 8;
    streamDesc.mBytesPerFrame = 8;
    
    UInt32 one = 1;
    AudioUnitSetProperty(outUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Input, 1, &one, sizeof(one));
    AudioUnitSetProperty(outUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Output, 0, &one, sizeof(one));
    
    AudioUnitSetProperty(outUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &streamDesc, sizeof(streamDesc));
    AudioUnitSetProperty(outUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &streamDesc, sizeof(streamDesc));
    
    AURenderCallbackStruct callbackStruct;
    //callbackStruct.inputProc = recordingCallback;
    //callbackStruct.inputProcRefCon = port;
    //AudioUnitSetProperty(outUnit, kAudioOutputUnitProperty_SetInputCallback, kAudioUnitScope_Output, 1, &callbackStruct, sizeof(callbackStruct));
    
    callbackStruct.inputProc = &renderCallback;
    callbackStruct.inputProcRefCon = (__bridge void *)self;
    AudioUnitSetProperty(outUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &callbackStruct, sizeof(callbackStruct));
    
    OSStatus result = AUGraphInitialize(processingGraph);
    AUGraphStart(processingGraph);
    */
}

- (void)receive
{
    char *buffer = (char *)malloc(sizeof(char) * 1000000);
    Xmodem xmodem(port);
    //xmodem.recv(NULL);
    
    int bytesReceived = xmodem.recv((char *)buffer);
    
    NSString *docsDir = [ViewController applicationDocumentsDirectory];
    NSString *audioFile = [docsDir stringByAppendingPathComponent:@"filerec.bin"];
    FILE *fp = fopen([audioFile cStringUsingEncoding:NSUTF8StringEncoding], "wb");
    
    fwrite(buffer, 1, bytesReceived, fp);
    fclose(fp);
    printf("File complete: %s\n", [audioFile cStringUsingEncoding:NSUTF8StringEncoding]);
    free(buffer);
    
    
    //unsigned char test[100];
    //sprintf((char *)test, "Howza poopen my friend!");
    //port->send(test, strlen((char *)test));
}

- (void)layout
{
    float width = [[UIScreen mainScreen] bounds].size.width;
    float height = [[UIScreen mainScreen] bounds].size.height;
    
    if (UIInterfaceOrientationIsLandscape(self.interfaceOrientation))
    {
        float temp = width;
        width = height;
        height = temp;
    }
    
    textView.frame = CGRectMake(0, 0, width, height/2);
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

- (BOOL) shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
    NSLog(@"called!");
    return YES;
}

- (void) didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
    [self layout];
}

/*
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
    // TEST TEST
    OSStatus err = noErr;
    ViewController *vc = (__bridge ViewController *)inRefCon;
    err = AudioUnitRender(vc->audioUnit, ioActionFlags, inTimeStamp, 1, inNumberFrames, ioData);
    AudioBuffer *buf = &ioData->mBuffers[0];
    
    printf("sz %ld\n", buf->mDataByteSize);
    float *res = (float *)buf->mData;
    printf("%f\n", res[0]);
    //===
    
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
     
    return 0;
}
*/

/*
static OSStatus recordingCallback(void *							inRefCon,
                                  AudioUnitRenderActionFlags *	ioActionFlags,
                                  const AudioTimeStamp *			inTimeStamp,
                                  UInt32							inBusNumber,
                                  UInt32							inNumberFrames,
                                  AudioBufferList * __nullable	ioData)
{
    
    //printf("record\n");
    
    AudioBufferList bufferList;
    bufferList.mNumberBuffers = 1;
    bufferList.mBuffers[0].mDataByteSize = 2 * sizeof(float) * inNumberFrames;
    bufferList.mBuffers[0].mNumberChannels = 2;
    bufferList.mBuffers[0].mData = malloc(2 * sizeof(float) * inNumberFrames);
    
    ViewController *vc = (__bridge ViewController *)inRefCon;
    AudioUnitRender(vc->audioUnit,
                    ioActionFlags,
                    inTimeStamp,
                    inBusNumber,
                    inNumberFrames,
                    &bufferList);
    
    float* result = (float*)&bufferList.mBuffers[0].mData;
    printf("%f\n", result[0]);
    
    free(bufferList.mBuffers[0].mData);
    
    return noErr;
}
*/

@end
