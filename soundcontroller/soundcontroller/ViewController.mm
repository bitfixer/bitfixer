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
    net::Socket socket;
    FrequencyDetector *freqDetector;
    NSMutableArray *buttons;
    int port;
    float tempo;
    bool shouldExit;
    dispatch_queue_t timerQueue;
    int tickIndex;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    buttons = [[NSMutableArray alloc] init];
    tempo = 120;
    tickIndex = 0;
    shouldExit = false;
    
    // create layout view
    UIView *view = [[UIView alloc] init];
    view.frame = CGRectMake(0, 0, self.view.frame.size.height, self.view.frame.size.width);
    view.backgroundColor = [UIColor grayColor];
    
    CGFloat screenWidth = self.view.frame.size.height;
    CGFloat screenHeight = self.view.frame.size.width;
    
    int numRows = 5;
    int numColumns = 16;
    float margin = screenWidth * 0.01;

    int nummargins = 2 + (numColumns-1);
    float totalMarginWidth = nummargins * margin;
    float buttonWidth = (screenWidth-totalMarginWidth) / numColumns;
    
    for (int c = 0; c < numColumns; c++)
    {
        ButtonHolder *holder = [[ButtonHolder alloc] init];
        holder.button = [[UIButton alloc] init];
        holder.button.tag = c;
        float x = margin + (margin + buttonWidth)*c;
        holder.button.frame = CGRectMake(x, 100, buttonWidth, buttonWidth);
        holder.button.backgroundColor = [UIColor blueColor];
        [view addSubview:holder.button];
        
        [holder.button addTarget:self action:@selector(buttonTapped:) forControlEvents:UIControlEventTouchUpInside];
        [buttons addObject:holder];
    }
    
    CGAffineTransform centerTrans = CGAffineTransformMakeTranslation(-self.view.frame.size.height/2.0, -self.view.frame.size.width/2.0);
    CGAffineTransform rotTrans = CGAffineTransformMakeRotation(-M_PI / 2.0);
    CGAffineTransform recenterTrans = CGAffineTransformMakeTranslation(self.view.frame.size.width/2.0, self.view.frame.size.height/2.0);
    CGAffineTransform combinedTrans = CGAffineTransformConcat(CGAffineTransformConcat(rotTrans, centerTrans), recenterTrans);
    view.transform = combinedTrans;
    [self.view addSubview:view];
    
    // Do any additional setup after loading the view, typically from a nib.
    freqDetector = new FrequencyDetector(44100.0, 1500.0, 256);
    
    port = 30000;
    if ( !socket.Open( port ) )
    {
        printf( "failed to create socket!\n" );
    }
    
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
}

- (void)updateUI
{
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
}

- (void)runTimer
{
    while (!shouldExit)
    {
        tickIndex++;
        if (tickIndex >= 16)
        {
            tickIndex = 0;
        }
        //NSLog(@"tick");
        dispatch_async(dispatch_get_main_queue(), ^{
            [self updateUI];
        });
        float tickTime = 60.0 / (tempo * 4)  ;
        int usec = (int)(tickTime * 1000000.0);
        usleep(usec);
    }
}

- (void)buttonTapped:(UIButton *)button
{
    int index = button.tag;
    NSLog(@"button was tapped! %d", index);
    
    ButtonHolder *holder = [buttons objectAtIndex:index];
    if (holder.selected)
        holder.selected = NO;
    else
        holder.selected = YES;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
