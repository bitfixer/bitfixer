//
//  ScaleFilter.m
//  filtervr
//
//  Created by Michael Hill on 4/16/15.
//  Copyright (c) 2015 Michael Hill. All rights reserved.
//

#import "ScaleFilter.h"

@implementation ScaleFilter

- (id)initWithOutputSize:(CGSize)outputSize
{
    self = [self init];
    if (self)
    {
        self.outputSize = outputSize;
    }
    return self;
}

- (CGSize)maximumOutputSize
{
    return CGSizeZero;
}

- (CGSize)outputFrameSize
{
    return self.outputSize;
}

@end
