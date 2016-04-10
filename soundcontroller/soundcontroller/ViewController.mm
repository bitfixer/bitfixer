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

@interface ViewController ()

@end

@implementation ViewController
{
    Novocaine *audioManager;
    
    
    //DCT *dct;
    //float *output;
    //float *input;
    //int dct_length;
    //float *freq_table;
    net::Socket socket;
    FrequencyDetector *freqDetector;
    int port;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    /*
    dct_length = 64;
    dct = new DCT(dct_length);
    output = (float *)malloc(sizeof(float) * dct_length);
    input = (float *)malloc(sizeof(float) * dct_length);
    freq_table = (float *)malloc(sizeof(float) * dct_length);
    
    __block int input_samples_per_dct_sample = 1024 / dct_length;
    
    // create frequency table
    float fs = 44100.0 / input_samples_per_dct_sample;
    float maxfreq = fs / 2.0;
    float f_inc = maxfreq / (dct_length-1);
    for (int i = 0; i < dct_length; i++)
    {
        freq_table[i] = f_inc * i;
    }
    */
    
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
        
        double now = CACurrentMediaTime();
        double diff = now - time;
        printf("Time is %lf\tf %f\n", diff, topfreq);
        time = now;
        
        /*
        for (int i = 0; i < dct_length; i++)
        {
            vc->input[i] = data[i*input_samples_per_dct_sample];
        }
        
        vc->dct->getDCT(vc->input, vc->output, vc->dct_length);
        
        float max = -9999.0;
        int index = 0;
        for (int i = 0; i < vc->dct_length; i++)
        {
            float thisdct = vc->output[i];
            if (thisdct < 0)
                thisdct = -thisdct;
            
            if (thisdct > max)
            {
                max = thisdct;
                index = i;
            }
        }
        
        double now = CACurrentMediaTime();
        
        double diff = now - time;
        
        if (max > 0.025)
        {
            printf("Time is %lf : \tmax %f index %d freq %f\n", diff, max, index, vc->freq_table[index]);
            vc->socket.Send(net::Address(10,0,0,5,vc->port), &vc->freq_table[index], sizeof(float));
        }
        
        time = now;
        */
    }];
    
    [audioManager play];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
