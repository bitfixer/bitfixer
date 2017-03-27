//
//  ViewController.h
//  filtervr
//
//  Created by Michael Hill on 4/5/15.
//  Copyright (c) 2015 Michael Hill. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GPUImage.h"
#import "AsciiFilter.h"

@interface ViewController : UIViewController <AsciiConverterDelegate>
{
    GPUImageVideoCamera *videoCamera;
    GPUImageOutput<GPUImageInput> *filter;
}

@end

