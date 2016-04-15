//
//  ViewController.m
//  soundcontroller
//
//  Created by Michael Hill on 4/9/16.
//  Copyright (c) 2016 Michael Hill. All rights reserved.
//

#import "ViewController.h"
#import "Novocaine.h"
#include "DCT.h"
#include "Net.h"
#include "FrequencyDetector.h"
#include "NetPort.h"
#include "SidController.h"
#include "timer.hpp"

@interface ButtonHolder : NSObject

@property (strong, nonatomic) UIButton *button;
@property (assign, nonatomic) BOOL selected;

@end

@implementation ButtonHolder

- (id) init
{
    self = [super init];
    self.selected = false;
    return self;
}

@end


@interface ViewController ()

@end

@implementation ViewController
{
    Novocaine *audioManager;
    FrequencyDetector *freqDetector;
    NetPort *netPort;
    SidController *controller;
    
    Timer timer;
    
    NSMutableArray *buttonRows;
    NSMutableArray *controlRows;
    
    int numColumns;
    int port;
    float tempo;
    bool shouldExit;
    dispatch_queue_t timerQueue;
    int tickIndex;
    
    long currTick;
    double netTime;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    //buttons = [[NSMutableArray alloc] init];
    buttonRows = [[NSMutableArray alloc] init];
    controlRows = [[NSMutableArray alloc] init];
    tempo = 120;
    tickIndex = 0;
    shouldExit = false;
    
    // create layout view
    UIView *view = [[UIView alloc] init];
    view.frame = CGRectMake(0, 0, self.view.frame.size.height, self.view.frame.size.width);
    view.backgroundColor = [UIColor grayColor];
    
    CGFloat screenWidth = self.view.frame.size.height;
    CGFloat screenHeight = self.view.frame.size.width;
    
    int numRows = 3;
    numColumns = 16;
    float margin = screenWidth * 0.01;

    int nummargins = 2 + (numColumns-1);
    float totalMarginWidth = nummargins * margin;
    float buttonWidth = (screenWidth-totalMarginWidth) / numColumns;
    
    float buttonOffset = 500.0;
    for (int r = 0; r < numRows; r++)
    {
        float rowY = buttonOffset + (r+1)*margin + (r * buttonWidth);
        NSMutableArray *buttons = [[NSMutableArray alloc] init];
        for (int c = 0; c < numColumns; c++)
        {
            ButtonHolder *holder = [[ButtonHolder alloc] init];
            holder.button = [[UIButton alloc] init];
            holder.button.tag = r*numColumns + c;
            float x = margin + (margin + buttonWidth)*c;
            holder.button.frame = CGRectMake(x, rowY, buttonWidth, buttonWidth);
            holder.button.backgroundColor = [UIColor blueColor];
            [view addSubview:holder.button];
            
            [holder.button addTarget:self action:@selector(buttonTapped:) forControlEvents:UIControlEventTouchUpInside];
            [buttons addObject:holder];
        }
        [buttonRows addObject:buttons];
    }
    
    // create channel control buttons
    for (int r = 0; r < numRows; r++)
    {
        float rowY = 100 + (r+1)*margin + (r*buttonWidth);
        NSMutableArray *buttons = [[NSMutableArray alloc] init];
        for (int i = 0; i < 4; i++)
        {
            ButtonHolder *holder = [[ButtonHolder alloc] init];
            holder.button = [[UIButton alloc] init];
            holder.button.tag = r*4 + i;
            float x = margin + (margin + buttonWidth)*i;
            holder.button.frame = CGRectMake(x, rowY, buttonWidth, buttonWidth);
            holder.button.backgroundColor = [UIColor blueColor];
            [view addSubview:holder.button];
            
            if (i == 0)
            {
                holder.selected = true;
            }
            
            [holder.button addTarget:self action:@selector(controlButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
            [buttons addObject:holder];
        }
        [controlRows addObject:buttons];
    }
    
    
    // add tempo slider
    UISlider *tempoSlider = [[UISlider alloc] init];
    tempoSlider.frame = CGRectMake(0, 0, 200, 100);
    [tempoSlider addTarget:self action:@selector(sliderSlid:) forControlEvents:UIControlEventValueChanged];
    [view addSubview:tempoSlider];
    
    // add frequency sliders
    UISlider *freqSlider = [[UISlider alloc] init];
    freqSlider.frame = CGRectMake(200, 0, 200, 100);
    freqSlider.tag = 0;
    [freqSlider addTarget:self action:@selector(freqSlider:) forControlEvents:UIControlEventValueChanged];
    [view addSubview:freqSlider];
    
    freqSlider = [[UISlider alloc] init];
    freqSlider.frame = CGRectMake(400, 0, 200, 100);
    freqSlider.tag = 1;
    [freqSlider addTarget:self action:@selector(freqSlider:) forControlEvents:UIControlEventValueChanged];
    [view addSubview:freqSlider];
    
    CGAffineTransform centerTrans = CGAffineTransformMakeTranslation(-self.view.frame.size.height/2.0, -self.view.frame.size.width/2.0);
    CGAffineTransform rotTrans = CGAffineTransformMakeRotation(-M_PI / 2.0);
    CGAffineTransform recenterTrans = CGAffineTransformMakeTranslation(self.view.frame.size.width/2.0, self.view.frame.size.height/2.0);
    CGAffineTransform combinedTrans = CGAffineTransformConcat(CGAffineTransformConcat(rotTrans, centerTrans), recenterTrans);
    view.transform = combinedTrans;
    [self.view addSubview:view];
    
    // Do any additional setup after loading the view, typically from a nib.
    freqDetector = new FrequencyDetector(44100.0, 1500.0, 256);
    
    // initialize SID
    //169.254.85.110
    netPort = new NetPort(192, 168, 0, 109, 29999);
    controller = new SidController(netPort);
    
    audioManager = [Novocaine audioManager];
    __weak ViewController *wself = self;
    
    __block double time = CACurrentMediaTime();
    [audioManager setInputBlock:^(float *data, UInt32 numFrames, UInt32 numChannels) {
        ViewController *vc = (ViewController *)wself;
        
        vc->freqDetector->detect(data, numFrames);
        float topfreq = vc->freqDetector->getTopFrequency();
    }];
    
    [audioManager play];
    
    // start timer thread
    timerQueue = dispatch_queue_create("timerQueue", 0);
    dispatch_async(timerQueue, ^{
        ViewController *vc = (ViewController *)wself;
        [wself runTimer];
    });
    
    [self updateUI];
}

- (void)updateUI
{
    int numRows = [buttonRows count];
    
    for (int r = 0; r < numRows; r++)
    {
        NSMutableArray *buttons = [buttonRows objectAtIndex:r];
        for (int i = 0; i < 16; i++)
        {
            ButtonHolder *holder = [buttons objectAtIndex:i];
            if (i == tickIndex)
            {
                if (holder.selected)
                    [holder.button setBackgroundColor:[UIColor orangeColor]];
                else
                    [holder.button setBackgroundColor:[UIColor greenColor]];
            }
            else
            {
                if (holder.selected)
                    [holder.button setBackgroundColor:[UIColor redColor]];
                else
                    [holder.button setBackgroundColor:[UIColor blueColor]];
            }
        }

        NSMutableArray *controlRow = [controlRows objectAtIndex:r];
        // update control buttons
        for (int i = 0; i < 4; i++)
        {
            ButtonHolder *holder = [controlRow objectAtIndex:i];
            if (holder.selected)
            {
                [holder.button setBackgroundColor:[UIColor redColor]];
            }
            else
            {
                [holder.button setBackgroundColor:[UIColor blueColor]];
            }
        }
        
        
    }
}

- (void)setTempo:(float)newTempo
{
    @synchronized(self)
    {
        tempo = newTempo;
        currTick = 0;
        netTime = netPort->getTime();
    }
}

- (void)setFreq:(float)freq forVoice:(int)v
{
    @synchronized(self)
    {
        printf("voice %d is freq %f", v, freq);
        controller->setFrequency(v, freq);
    }
}

- (void)runTimer
{
    controller->init();
    controller->setVolume(1.0);
    
    for (int i = 0; i < 3; i++)
    {
        controller->setFrequency(i, 440.0*(i+1));
        controller->setSustain(i, 0.85);
        controller->setRelease(i, 0.1);
        controller->setWaveform(i, Sid::waveForm::TRIANGLE);
    }
    
    /*
    controller->setFrequency(2, 220.0);
    controller->setSustain(2, 0.85);
    controller->setRelease(2, 0.1);
    controller->setWaveform(2, Sid::waveForm::NOISE);
    */
     
    [self setTempo:tempo];
    while (!shouldExit)
    {
        int usec;
        @synchronized(self)
        {
            tickIndex++;
            if (tickIndex >= 16)
            {
                tickIndex = 0;
            }
            
            double tickTime;
            double nextTick;
            double tt;
            double waitTime;
            
            tt = netPort->getTime();
            currTick++;
            tickTime = 60.0 / (tempo * 4);
            nextTick = ((double)currTick * tickTime) + netTime;
            waitTime = nextTick - tt;
            if (waitTime < 0.0)
            {
                waitTime = tickTime;
            }
            
            printf("waittime %lf\n", waitTime);
            usec = (int)(waitTime * 1000000.0);
            
            // set delay param of net port
            netPort->delay = waitTime;
            
            int numRows = [buttonRows count];
            for (int r = 0; r < numRows; r++)
            {
                NSMutableArray *buttons = [buttonRows objectAtIndex:r];
                
                // send note update
                ButtonHolder *holder = [buttons objectAtIndex:tickIndex];
                if (holder.selected)
                {
                    // send note on
                    controller->noteOn(r);
                }
                else
                {
                    controller->noteOff(r);
                }
            }
        }
        usleep(usec);
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [self updateUI];
        });
    }
}

- (void)buttonTapped:(UIButton *)button
{
    int i = button.tag;
    int col = i % numColumns;
    int row = (i-col) / numColumns;
    
    NSLog(@"button was tapped! %d %d", row, col);
    
    NSMutableArray *buttons = [buttonRows objectAtIndex:row];
    
    ButtonHolder *holder = [buttons objectAtIndex:col];
    if (holder.selected)
        holder.selected = NO;
    else
        holder.selected = YES;
}

- (void)controlButtonTapped:(UIButton *)button
{
    int i = button.tag;
    int col = i % 4;
    int row = (i-col) / 4;
    
    NSMutableArray *buttons = [controlRows objectAtIndex:row];
    
    // clear selection for row
    for (int i = 0; i < [buttons count]; i++)
    {
        ButtonHolder *holder = [buttons objectAtIndex:i];
        holder.selected = NO;
    }
    
    ButtonHolder *holder = [buttons objectAtIndex:col];
    holder.selected = YES;
    
    // send control signal
    if (col == 0)
    {
        controller->setWaveform(row, Sid::waveForm::TRIANGLE);
    }
    else if (col == 1)
    {
        controller->setWaveform(row, Sid::waveForm::SAWTOOTH);
    }
    else if (col == 2)
    {
        controller->setWaveform(row, Sid::waveForm::PULSE);
    }
    else if (col == 3)
    {
        controller->setWaveform(row, Sid::waveForm::NOISE);
    }
    
    [self updateUI];
}

- (void)sliderSlid:(UISlider *)slider
{
    float val = [slider value];
    printf("value is %f\n", val);
    
    float minTempo = 50.0;
    float maxTempo = 200.0;
    
    float newTempo = minTempo + (maxTempo-minTempo)*val;
    [self setTempo:newTempo];
}

- (void)freqSlider:(UISlider *)slider
{
    float val = [slider value];
    float minFreq = 50.0;
    float maxFreq = 1499.0;
    float newFreq = minFreq + (maxFreq-minFreq)*val;
    int voice = slider.tag;
    [self setFreq:newFreq forVoice:voice];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
