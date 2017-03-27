//
//  C64Filter.m
//  filtervr
//
//  Created by Michael Hill on 3/26/17.
//  Copyright © 2017 Michael Hill. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "C64Filter.h"
#include "Image.hpp"
#include "Ditherer.hpp"
#include "C64Image.hpp"
#include "c64_colors.h"
#include "timer.hpp"

@implementation C64Filter
{
    int texHeight;
    int texWidth;
    Palette* c64palette;
    Ditherer* ditherer;
    unsigned char* c64frame;
    Tools::Timer timer;
}

- (id) init
{
    self = [super init];
    if (self)
    {
        c64palette = new Palette(c64_colors, num_64_colors);
        ditherer = Ditherer::createC64Ditherer();
        c64frame = NULL;
    }
    
    return self;
}

- (void) dealloc
{
    
}

- (void)setupFilterForSize:(CGSize)filterFrameSize
{
    if (!self.initialized)
    {
        _initialized = YES;
        texWidth = (int)filterFrameSize.width;
        texHeight = (int)filterFrameSize.height;
        NSLog(@"C64 setupFilter size %d %d", texWidth, texHeight);
    }
}

- (void)renderToTextureWithVertices:(const GLfloat *)vertices textureCoordinates:(const GLfloat *)textureCoordinates;
{
    if (self.preventRendering)
    {
        [firstInputFramebuffer unlock];
        return;
    }
    
    [GPUImageContext setActiveShaderProgram:filterProgram];
    
    outputFramebuffer = [[GPUImageContext sharedFramebufferCache] fetchFramebufferForSize:[self sizeOfFBO] textureOptions:self.outputTextureOptions onlyTexture:NO];
    [outputFramebuffer activateFramebuffer];
    if (usingNextFrameForImageCapture)
    {
        [outputFramebuffer lock];
    }
    
    [self setUniformsForProgramAtIndex:0];
    glFinish();
    
    GLubyte *inputBytes = [firstInputFramebuffer byteBuffer];
    
    // create c64 image
    CGSize fbSize = firstInputFramebuffer.size;
    
    timer.start();
    Image inputImage((int)fbSize.width, (int)fbSize.height, (int)[firstInputFramebuffer bytesPerRow], 4, inputBytes);
    double t1 = timer.getTime();
    NSURL* docsDir = [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] lastObject];
    NSURL* ppmurl = [docsDir URLByAppendingPathComponent:@"output.ppm"];
    
    // output the image
    //inputImage.writePPM([[ppmurl path] cStringUsingEncoding:NSUTF8StringEncoding]);
    Image halfImage(inputImage, 160, 200);
    Image* dithered = ditherer->createDitheredImageFromImageWithPalette(halfImage, *c64palette);
    C64Image* c64im = (C64Image*)dithered;
    double t = timer.getTime();
    NSLog(@"Image converted %lf %lf",t1, t);
    
    int c64FrameSize = c64im->getC64FrameSize();
    if (!c64frame)
    {
        c64frame = (unsigned char*)malloc(c64FrameSize);
    }
    float time = 0.0;
    c64im->getC64Frame(c64frame, time);
    NSData* c64Data = [NSData dataWithBytes:&c64frame[4] length:c64FrameSize-4];
    if (self.delegate)
    {
        [self.delegate asciiConverterGotFrame:c64Data];
    }
    
    delete dithered;
    
}

- (CGSize)maximumOutputSize
{
    return CGSizeZero;
}

@end
