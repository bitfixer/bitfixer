//
//  AsciiFilter.m
//  filtervr
//
//  Created by Michael Hill on 4/5/15.
//  Copyright (c) 2015 Michael Hill. All rights reserved.
//

#import "AsciiFilter.h"

@implementation AsciiFilter
{
    GLuint textureID;
    GLubyte *textureBytes;
    GLubyte *procTexBytes;
    //GLubyte *inputBytes;
    int texHeight, texWidth, frameSize;
    double *dctOutput;
    unsigned char *matchingGlyphs;
}

- (id)init
{
    self = [super init];
    if (self)
    {
        self.dctMatcher = [[DCTMatcher alloc] init];
        dctOutput = (double *)malloc(sizeof(double) * 64);
    }
    return self;
}

- (void)dealloc
{
    if (dctOutput)
        free(dctOutput);
}

- (void)setupFilterForSize:(CGSize)filterFrameSize
{
    if (!self.initialized)
    {
        NSLog(@"frame size is: %f, %f", filterFrameSize.width, filterFrameSize.height);
        
        _initialized = YES;
        texWidth = (int)filterFrameSize.width;
        texHeight = (int)filterFrameSize.height;
        
        matchingGlyphs = (unsigned char *)malloc(sizeof(unsigned char) * (texWidth/8) * (texHeight/8));
        //frameSize = (texWidth/8) * (texHeight/8);
        frameSize = 40 * 25;
        
        textureBytes = (GLubyte *)calloc(filterFrameSize.width * filterFrameSize.height, sizeof(GLubyte));
        procTexBytes = (GLubyte *)calloc(filterFrameSize.width * filterFrameSize.height, sizeof(GLubyte));
        //inputBytes = (GLubyte *)calloc(filterFrameSize.width * 2 * filterFrameSize.height * 4, sizeof(GLubyte));
        
        memset(textureBytes, 128, filterFrameSize.width * filterFrameSize.height * sizeof(GLubyte));
        
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, filterFrameSize.width, filterFrameSize.height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, textureBytes);
        glBindTexture(GL_TEXTURE_2D, 0);
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
    
    // get bytes from input texture
    glFinish();
    //GLubyte *inputBytes = [firstInputFramebuffer byteBuffer] + 1; // use green color
    GLubyte *inputBytes = [firstInputFramebuffer byteBuffer];
    float colorVal;
    float scale = sqrtf(3.0);
    
    //glReadPixels(0, 0, texWidth, texHeight, GL_RGBA, GL_UNSIGNED_BYTE, inputBytes);
    
    GLubyte *texPtr = textureBytes;
    for (int i = 0; i < texHeight; i++)
    {
        GLubyte *rowPtr = inputBytes + (i * [firstInputFramebuffer bytesPerRow]);
        for (int j = 0; j < texWidth; j++)
        {
            colorVal = sqrtf((float)rowPtr[0]*rowPtr[0] + (float)rowPtr[1]*rowPtr[1] + (float)rowPtr[2]*rowPtr[2]) / scale;
            //*texPtr = *rowPtr;
            *texPtr = (GLubyte)floorf(colorVal);
            texPtr++;
            rowPtr += 4;
        }
    }
    
    //printf("bytes[1000] = %d\n", textureBytes[1000]);
    
    int glyphIndex = 0;
    int vmargin = (texHeight/8 - 25) / 2;
    int vend = vmargin + 25;
    // do glyph matching
    
    int hline = 0;
    for (int h = 0; h < texHeight; h += 8)
    {
        for (int w = 0; w < texWidth; w += 8)
        {
            [self.dctMatcher dctSubsectionFromBuffer:textureBytes xstart:w y:h width:texWidth output:dctOutput];
            int matching = [self.dctMatcher getMatchingGlyph:dctOutput];
            
            if (hline >= vmargin && hline < vend)
            {
                matchingGlyphs[glyphIndex] = (unsigned char)matching;
                glyphIndex++;
            }
                
            const unsigned char *glyph = [self.dctMatcher getGlyphAtIndex:matching];
            
            for (int y = 0; y < 8; y++)
            {
                int pixelIndex = (h+y)*texWidth + w;
                memcpy(&procTexBytes[pixelIndex], glyph, 8);
                glyph += 8;
            }
        }
        hline++;
    }
    
    NSData *asciiFrame = [NSData dataWithBytes:matchingGlyphs length:frameSize];
    if (self.delegate)
        [self.delegate asciiConverterGotFrame:asciiFrame];
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, texWidth, texHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, procTexBytes);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glClearColor(backgroundColorRed, backgroundColorGreen, backgroundColorBlue, backgroundColorAlpha);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glActiveTexture(GL_TEXTURE2);
    //glBindTexture(GL_TEXTURE_2D, [firstInputFramebuffer texture]);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glUniform1i(filterInputTextureUniform, 2);
    
    glVertexAttribPointer(filterPositionAttribute, 2, GL_FLOAT, 0, 0, vertices);
    glVertexAttribPointer(filterTextureCoordinateAttribute, 2, GL_FLOAT, 0, 0, textureCoordinates);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    [firstInputFramebuffer unlock];
    
    if (usingNextFrameForImageCapture)
    {
        dispatch_semaphore_signal(imageCaptureSemaphore);
    }
}

- (CGSize)maximumOutputSize
{
    //return CGSizeMake(320.0, 320.0);
    //return CGSizeMake(160.0, 160.0);
    return CGSizeZero;
}

@end
