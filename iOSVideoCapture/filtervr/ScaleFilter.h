//
//  ScaleFilter.h
//  filtervr
//
//  Created by Michael Hill on 4/16/15.
//  Copyright (c) 2015 Michael Hill. All rights reserved.
//

#import <GPUImage.h>

@interface ScaleFilter : GPUImageCropFilter

- (id)initWithOutputSize:(CGSize)outputSize;
- (CGSize)maximumOutputSize;
- (CGSize)outputFrameSize;

@property (nonatomic, assign) CGSize outputSize;

@end
