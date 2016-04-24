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
    int numControls;
    int port;
    float tempo;
    bool shouldExit;
    dispatch_queue_t timerQueue;
    int tickIndex;
    
    long currTick;
    double netTime;
    
    BOOL harmonize;
    
    float samples[5000000];
    int numsamples;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    buttonRows = [[NSMutableArray alloc] init];
    controlRows = [[NSMutableArray alloc] init];
    tempo = 120;
    tickIndex = 0;
    shouldExit = false;
    numControls = 3;
    harmonize = NO;
    numsamples = 0;
    
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
    
    float textHeight = 50;
    float controlLabelY = 100;
    float controlRowY = controlLabelY + textHeight;
    
    float adsrControlWidth = 100.0;
    float adsrControlX = view.frame.size.width - (5*margin + 4*adsrControlWidth);
    
    for (int c = 0; c < numRows; c++)
    {
        UILabel *label = [[UILabel alloc] init];
        label.backgroundColor = [UIColor colorWithRed:0.2 green:0.2 blue:0.2 alpha:1.0];
        label.textColor = [UIColor whiteColor];
        label.textAlignment = NSTextAlignmentCenter;
        if (c == 0)
        {
            label.text = @"TRI";
        }
        else if (c == 1)
        {
            label.text = @"SAW";
        }
        else if (c == 2)
        {
            label.text = @"NOI";
        }
        label.frame = CGRectMake((c+1)*margin + (c*buttonWidth), controlLabelY, buttonWidth, buttonWidth);
        [view addSubview:label];
    }
    
    // create channel control buttons
    for (int r = 0; r < numRows; r++)
    {
        float rowY = controlRowY + (r+1)*margin + (r*buttonWidth);
        NSMutableArray *buttons = [[NSMutableArray alloc] init];
        for (int i = 0; i < numControls; i++)
        {
            ButtonHolder *holder = [[ButtonHolder alloc] init];
            holder.button = [[UIButton alloc] init];
            holder.button.tag = r*numControls + i;
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
    
    float currAdsrControlX = adsrControlX;
    for (int i = 0; i < 4; i++)
    {
        UILabel *label = [[UILabel alloc] init];
        label.backgroundColor = [UIColor colorWithRed:0.2 green:0.2 blue:0.2 alpha:1.0];
        label.textColor = [UIColor whiteColor];
        label.textAlignment = NSTextAlignmentCenter;
        
        if (i == 0)
        {
            label.text = @"A";
        }
        else if (i == 1)
        {
            label.text = @"D";
        }
        else if (i == 2)
        {
            label.text = @"S";
        }
        else if (i == 3)
        {
            label.text = @"R";
        }
        
        currAdsrControlX += margin;
        label.frame = CGRectMake(currAdsrControlX, controlLabelY, adsrControlWidth, textHeight);
        [view addSubview:label];
        
        float currSliderY = controlLabelY + textHeight;
        
        float freqSliderX = (numControls+1)*margin + numControls*buttonWidth;
        float freqSliderWidth = adsrControlX - freqSliderX;
        
        label = [[UILabel alloc] init];
        label.frame = CGRectMake(freqSliderX, controlLabelY, freqSliderWidth, textHeight);
        label.backgroundColor = [UIColor colorWithRed:0.2 green:0.2 blue:0.2 alpha:1.0];
        label.textColor = [UIColor whiteColor];
        label.textAlignment = NSTextAlignmentCenter;
        label.text = @"Frequency";
        [view addSubview:label];
        
        for (int s = 0; s < numRows; s++)
        {
            // create sliders for channels
            UISlider *slider = [[UISlider alloc] init];
            currSliderY += margin;
            slider.frame = CGRectMake(currAdsrControlX, currSliderY, adsrControlWidth, buttonWidth);
            slider.tag = s*4 + i;
            [slider addTarget:self action:@selector(adsrSlider:) forControlEvents:UIControlEventValueChanged];
            [view addSubview:slider];
            
            if (i == 0)
            {
                // add frequency slider
                UISlider *freqSlider = [[UISlider alloc] init];
                freqSlider.frame = CGRectMake(freqSliderX, currSliderY, freqSliderWidth, buttonWidth);
                freqSlider.tag = s;
                [freqSlider addTarget:self action:@selector(freqSlider:) forControlEvents:UIControlEventValueChanged];
                [view addSubview:freqSlider];
            }
            
            currSliderY += buttonWidth;
        }
        
        currAdsrControlX += adsrControlWidth;
        
    }
    
    // add reset button
    float resetButtonWidth = 100.0;
    float resetButtonX = view.frame.size.width - (margin+resetButtonWidth);
    UIButton *resetButton = [[UIButton alloc] init];
    resetButton.backgroundColor = [UIColor blueColor];
    [resetButton setTitle:@"Reset" forState:UIControlStateNormal];
    [resetButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    resetButton.frame = CGRectMake(resetButtonX, margin, resetButtonWidth, buttonWidth);
    [resetButton addTarget:self action:@selector(resetButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [view addSubview:resetButton];
    
    UILabel *tempoLabel = [[UILabel alloc] init];
    tempoLabel.frame = CGRectMake(margin, margin, (numControls-1)*margin + numControls*buttonWidth, buttonWidth);
    tempoLabel.backgroundColor = [[UIColor alloc] initWithRed:0.2 green:0.2 blue:0.2 alpha:1.0];
    tempoLabel.textColor = [UIColor whiteColor];
    tempoLabel.textAlignment = NSTextAlignmentCenter;
    tempoLabel.text = @"Tempo";
    [view addSubview:tempoLabel];
    
    float tempoSliderX = (numControls+1)*margin + numControls*buttonWidth;
    float tempoSliderWidth = adsrControlX - tempoSliderX;
    // add tempo slider
    UISlider *tempoSlider = [[UISlider alloc] init];
    tempoSlider.frame = CGRectMake(tempoSliderX, margin, tempoSliderWidth, buttonWidth);
    [tempoSlider addTarget:self action:@selector(sliderSlid:) forControlEvents:UIControlEventValueChanged];
    [view addSubview:tempoSlider];
    
    CGAffineTransform centerTrans = CGAffineTransformMakeTranslation(-self.view.frame.size.height/2.0, -self.view.frame.size.width/2.0);
    CGAffineTransform rotTrans = CGAffineTransformMakeRotation(-M_PI / 2.0);
    CGAffineTransform recenterTrans = CGAffineTransformMakeTranslation(self.view.frame.size.width/2.0, self.view.frame.size.height/2.0);
    CGAffineTransform combinedTrans = CGAffineTransformConcat(CGAffineTransformConcat(rotTrans, centerTrans), recenterTrans);
    view.transform = combinedTrans;
    [self.view addSubview:view];
    
    /*
    UIButton *harmonizeButton = [[UIButton alloc] init];
    harmonizeButton.backgroundColor = [UIColor colorWithRed:0.2 green:0.2 blue:0.2 alpha:1.0];
    harmonizeButton.titleLabel.textColor = [UIColor whiteColor];
    harmonizeButton.titleLabel.text = @"Harmonize";
    [harmonizeButton addTarget:self action:@selector(harmonizeStart:) forControlEvents:UIControlEventTouchDown];
    [harmonizeButton addTarget:self action:@selector(harmonizeEnd:) forControlEvents:UIControlEventTouchUpInside];
    harmonizeButton.frame = CGRectMake(margin, 400, 200, buttonWidth);
    [view addSubview:harmonizeButton];
    */
    // Do any additional setup after loading the view, typically from a nib.
    freqDetector = new FrequencyDetector(44100.0, 1500.0, 256);
    
    // initialize SID
    //169.254.85.110
    //netPort = new NetPort(192, 168, 0, 109, 29999);
    netPort = new NetPort(169,254,225,31, 29999);
    
    controller = new SidController(netPort);
    
    audioManager = [Novocaine audioManager];
    __weak ViewController *wself = self;
    
    __block double time = CACurrentMediaTime();
    [audioManager setInputBlock:^(float *data, UInt32 numFrames, UInt32 numChannels) {
        ViewController *vc = (ViewController *)wself;
        
        /*
        vc->freqDetector->detect(data, numFrames);
        float topfreq = vc->freqDetector->getTopFrequency();
        
        if (vc->harmonize && topfreq > 0.0)
        {
            vc->controller->setFrequency(0, topfreq * 1.5);
        }
        */
        
        if (harmonize == YES && numsamples + numFrames < 500000)
        {
            float *d = &samples[numsamples];
            for (int i = 0; i < numFrames/32; i++)
            {
                d[i] = data[i*32];
                numsamples++;
            }
            
            //memcpy(d, data, sizeof(float) * numFrames);
            //numsamples += numFrames;
        }
        
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
        for (int i = 0; i < numControls; i++)
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
        /*
        usleep(usec/2);
        int numRows = [buttonRows count];
        for (int r = 0; r < numRows; r++)
        {
            NSMutableArray *buttons = [buttonRows objectAtIndex:r];
            
            // send note update
            ButtonHolder *holder = [buttons objectAtIndex:tickIndex];
            if (holder.selected)
                controller->noteOff(r);
        }
        usleep(usec/2);
        */
        
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
    int col = i % numControls;
    int row = (i-col) / numControls;
    
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
        controller->setWaveform(row, Sid::waveForm::NOISE);
    }
    
    [self updateUI];
}

- (void)resetButtonTapped:(UIButton *)button
{
    // unselect all buttons
    int numRows = [buttonRows count];
    for (int r = 0; r < numRows; r++)
    {
        NSMutableArray *buttons = [buttonRows objectAtIndex:r];
        for (int b = 0; b < [buttons count]; b++)
        {
            ButtonHolder *holder = [buttons objectAtIndex:b];
            holder.selected = false;
        }
    }
    
    controller->init();
    controller->setVolume(1.0);
    
    for (int i = 0; i < 3; i++)
    {
        controller->setFrequency(i, 440.0*(i+1));
        controller->setSustain(i, 0.85);
        controller->setRelease(i, 0.1);
        controller->setWaveform(i, Sid::waveForm::TRIANGLE);
    }
    
    [self setTempo:tempo];
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

- (void)adsrSlider:(UISlider *)slider
{
    int i = slider.tag;
    int col = i % 4;
    int row = (i-col) / 4;
    
    // send control signal
    if (col == 0)
    {
        controller->setAttack(row, slider.value);
    }
    else if (col == 1)
    {
        controller->setDecay(row, slider.value);
    }
    else if (col == 2)
    {
        controller->setSustain(row, slider.value);
    }
    else if (col == 3)
    {
        controller->setRelease(row, slider.value);
    }
}

- (void)harmonizeStart:(UIButton *)button
{
    harmonize = YES;
    numsamples = 0;
}

- (void)harmonizeEnd:(UIButton *)button
{
    harmonize = NO;
    
    // play a sampled sound
    for (int i = 0; i < numsamples; i++)
    {
        controller->setVolume(samples[i]);
    }
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
