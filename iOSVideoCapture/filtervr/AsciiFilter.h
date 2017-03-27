//
//  AsciiFilter.h
//  filtervr
//
//  Created by Michael Hill on 4/5/15.
//  Copyright (c) 2015 Michael Hill. All rights reserved.
//

#import <GPUImage.h>
#import "DCTMatcher.h"

@protocol AsciiConverterDelegate <NSObject>
- (void)asciiConverterGotFrame:(NSData *)frame;
@end

@interface AsciiFilter : GPUImageFilter

- (void)setupFilterForSize:(CGSize)filterFrameSize;
- (void)renderToTextureWithVertices:(const GLfloat *)vertices textureCoordinates:(const GLfloat *)textureCoordinates;
- (CGSize)maximumOutputSize;

@property (nonatomic, readonly) BOOL initialized;
@property (nonatomic, strong) DCTMatcher *dctMatcher;
@property (nonatomic, assign) id<AsciiConverterDelegate> delegate;

@end
