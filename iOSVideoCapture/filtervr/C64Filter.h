//
//  C64Filter.h
//  filtervr
//
//  Created by Michael Hill on 3/26/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#import <GPUImage.h>
#import "AsciiFilter.h"

@interface C64Filter : GPUImageFilter

- (void)setupFilterForSize:(CGSize)filterFrameSize;
- (void)renderToTextureWithVertices:(const GLfloat *)vertices textureCoordinates:(const GLfloat *)textureCoordinates;
- (CGSize)maximumOutputSize;

@property (nonatomic, readonly) BOOL initialized;
@property (nonatomic, assign) id<AsciiConverterDelegate> delegate;

@end

