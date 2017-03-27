//
//  ViewController.m
//  filtervr
//
//  Created by Michael Hill on 4/5/15.
//  Copyright (c) 2015 Michael Hill. All rights reserved.
//

#import "ViewController.h"
#import "AsciiFilter.h"
#import "ScaleFilter.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

@interface ViewController ()

@end

@implementation ViewController
{
    int _socket;
    struct sockaddr_in _broadcastAddr;
    double lastSendTime;
    
    NSMutableArray *framesToSend;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.view.backgroundColor = [UIColor blackColor];
    
    _socket = -1;
    // Do any additional setup after loading the view, typically from a nib.
    videoCamera = [[GPUImageVideoCamera alloc] initWithSessionPreset:AVCaptureSessionPreset640x480
                                                      cameraPosition:AVCaptureDevicePositionBack];
    videoCamera.outputImageOrientation = UIInterfaceOrientationPortrait;
    
    //GPUImageCropFilter *crop = [[GPUImageCropFilter alloc] initWithCropRegion:CGRectMake(0, 0, 256.0/480.0, 256.0/640.0)];
    //ScaleFilter *crop = [[ScaleFilter alloc] initWithOutputSize:CGSizeMake(320.0, 320.0)];
    ScaleFilter *crop = [[ScaleFilter alloc] initWithOutputSize:CGSizeMake(320.0, 320.0)];
    crop.cropRegion = CGRectMake(0, 0, 1.0, 480.0/640.0);
    
    
    
    //GPUImageCropFilter *crop = [[GPUImageCropFilter alloc] initWithCropRegion:CGRectMake(0, 0, 320.0/480.0, 200.0/640.0)];
    [videoCamera addTarget:crop];
    
    GPUImageContrastFilter *contrast = [[GPUImageContrastFilter alloc] init];
    contrast.contrast = 0.75;
    [crop addTarget:contrast];
    
    GPUImageKuwaharaFilter *f = [[GPUImageKuwaharaFilter alloc] init];
    f.radius = 6;
    filter = f;
    
    //[scale addTarget:filter];
    //[crop addTarget:filter];
    [contrast addTarget:filter];
    
    GPUImageToonFilter *toonFilter = [[GPUImageToonFilter alloc] init];
    [filter addTarget:toonFilter];
    
    AsciiFilter *asciifilter = [[AsciiFilter alloc] init];
    asciifilter.delegate = self;
    [toonFilter addTarget:asciifilter];
    
    //GPUImageView *filterView = [[GPUImageView alloc] initWithFrame:self.view.frame];
    
    // shift image right or left by 3mm
    // center of image is 33.0mm from eye center, should be 30.0mm for cardboard ipd
    // screen width
    
    CGFloat screenWidth = self.view.frame.size.width;
    CGFloat horizCenterOffset = (3.0 / 59.0) * screenWidth;
    CGFloat newWidth = screenWidth * 0.75;
    
    CGFloat screenHeight = self.view.frame.size.height;
    
    CGFloat newHeight = screenHeight * 0.75;
    CGFloat vertOffset = newHeight - (newHeight*(480.0/640.0));
    
    CGFloat xDiff = screenWidth-newWidth;
    CGFloat yDiff = screenHeight-newHeight;
    
    //CGFloat yCenterOffset = (screenHeight-newHeight)*(480.0/640.0);
    
    //CGRect viewFrame = CGRectMake(-horizCenterOffset + xDiff, vertOffset + yDiff, newWidth, newHeight);
    CGRect viewFrame = CGRectMake(-horizCenterOffset + xDiff/2, yDiff+vertOffset, newWidth, newHeight);
    //CGRect viewFrame = self.view.frame;
    GPUImageView *filterView = [[GPUImageView alloc] initWithFrame:viewFrame];
    [self.view addSubview:filterView];
    
    framesToSend = [[NSMutableArray alloc] initWithCapacity:10];
    
    [asciifilter addTarget:filterView];
    //[contrast addTarget:filterView];
    [videoCamera startCameraCapture];
    
    lastSendTime = 0.0;
    
    // send frames
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        while (1)
        {
            if ([framesToSend count] >= 1)
            {
                NSData *data;
                @synchronized(self)
                {
                    data = [framesToSend objectAtIndex:0];
                    [framesToSend removeObjectAtIndex:0];
                }
                
                if (data)
                {
                    //[self sendPacket:data];
                }
            }
            else
            {
                usleep(10000);
            }
        }
    });
}

- (void)asciiConverterGotFrame:(NSData *)frame
{
    /*
    double timeNow = CACurrentMediaTime();
    double diff = timeNow - lastSendTime;
    
    if (diff > 1.0/10.0)
    {
        // send frame as udp packet
        [self sendPacket:frame];
        lastSendTime = timeNow;
    }
    */
    
    @synchronized(self)
    {
        //NSLog(@"got frame %d in queue", (int)[framesToSend count]);
        if ([framesToSend count] <= 5)
        {
            [framesToSend addObject:frame];
        }
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)sendPacket:(NSData *)data
{
    if (_socket == -1)
    {
        int sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sd<=0) {
            printf("Error: Could not open socket\n");
        }
        
        _socket = sd;
        
        /*
        // Set socket options
        // Enable broadcast
        int broadcastEnable=1;
        int ret=setsockopt(sd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
        if (ret) {
            printf("Error: Could not open set socket to broadcast mode\n");
            close(sd);
        }
        */
         
        memset(&_broadcastAddr, 0, sizeof _broadcastAddr);
        _broadcastAddr.sin_family = AF_INET;
        inet_pton(AF_INET, "10.0.0.9", &_broadcastAddr.sin_addr); // Set the broadcast IP address
        _broadcastAddr.sin_port = htons(9999);
    }
    
    printf("sending %d bytes\n", [data length]);
    int ret = (int)sendto(_socket, [data bytes], [data length], 0, (struct sockaddr*)&_broadcastAddr, sizeof _broadcastAddr);
    if (ret<0) {
        printf("Error: Could not open send broadcast\n");
        close(_socket);
    }
}

@end
