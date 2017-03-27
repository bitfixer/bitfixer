//
//  DCTMatcher.m
//  testcam
//
//  Created by Michael Hill on 3/28/15.
//
//

#import "DCTMatcher.h"

#define GLYPH_DIM       8
#define GLYPH_DIM_SQ    64
#define GLYPH_OUT_DIM   8
#define DCT_WEIGHT_LIMIT 0.3
#define WEIGHT_COEFF    0.5

//#define DCT_WEIGHT_LIMIT 0.0
//#define WEIGHT_COEFF 1.0

#define COLOR_LEVELS 128
#define COLOR_BIT_SHIFT 1


@implementation DCTMatcher
{
    double **dctWeights;
    double **dctInput;
    double *dctOutput;
    double **dctVert;
    double **dctSignatures;
    double **imagedat;
    double **alphalookup;
    double ****cosalphalookup;
    
    double *alphalookup1d;
    double **cosalphalookup1d;
    double ***pixBrightness;
    
    double *_sortedGlyphDCValues;
    int *_sortedGlyphIndices;
    
    int _dctCoeffCheckLimit;
    
    //double *glyphs;
    unsigned char *glyphs;
    
    const unsigned char *subsection[8];
}

- (id)init
{
    self = [super init];
    if (self)
    {
        int xDim = 320 / (8/GLYPH_DIM);
        int yDim = 200 / (8/GLYPH_DIM);
        
        imagedat = (double **)malloc(sizeof(double *) * xDim);
        for (int i = 0; i < xDim; i++)
        {
            imagedat[i] = (double *)malloc(sizeof(double) * yDim);
        }
        
        dctSignatures = (double **)malloc(sizeof(double *) * 256);
        for (int i = 0; i < 256; i++)
        {
            dctSignatures[i] = (double *)malloc(sizeof(double) * GLYPH_DIM_SQ);
        }
        
        dctInput = (double **)malloc(sizeof(double *) * GLYPH_DIM);
        for (int i = 0; i < GLYPH_DIM; i++)
        {
            dctInput[i] = (double *)malloc(sizeof(double) * GLYPH_DIM);
        }
        
        dctVert = (double **)malloc(sizeof(double *) * GLYPH_DIM);
        for (int i = 0; i < GLYPH_DIM; i++)
        {
            dctVert[i] = (double *)malloc(sizeof(double) * GLYPH_DIM);
        }
        
        dctOutput = (double *)malloc(sizeof(double) * GLYPH_DIM_SQ);
        
        alphalookup1d = (double *)malloc(sizeof(double) * GLYPH_DIM);
        for (int i = 0; i < GLYPH_DIM; i++)
        {
            alphalookup1d[i] = [self alpha:(double)i];
        }
        
        cosalphalookup1d = (double **)malloc(sizeof(double *) * GLYPH_DIM);
        for (int i = 0; i < GLYPH_DIM; i++)
        {
            cosalphalookup1d[i] = (double *)malloc(sizeof(double) * GLYPH_DIM);
        }
        
        for (int u = 0; u < GLYPH_DIM; u++)
        {
            for (int i = 0; i < GLYPH_DIM; i++)
            {
                cosalphalookup1d[u][i] = alphalookup1d[i] *
                cos(((M_PI*u)/(2*GLYPH_DIM))*(2*i + 1));
            }
        }
        
        
        alphalookup = (double **)malloc(sizeof(double *) * GLYPH_DIM);
        for (int i = 0; i < GLYPH_DIM; i++)
        {
            alphalookup[i] = (double *)malloc(sizeof(double) * GLYPH_DIM);
        }
        
        // populate alpha lookup table
        for(int i = 0; i < GLYPH_DIM; i++)
        {
            for(int j = 0; j < GLYPH_DIM; j++)
            {
                alphalookup[i][j] = [self alpha:(double)i] * [self alpha:(double)j];
            }
        }
        
        cosalphalookup = (double ****)malloc(sizeof(double ***) * GLYPH_DIM);
        for (int i = 0; i < GLYPH_DIM; i++)
        {
            cosalphalookup[i] = (double ***)malloc(sizeof(double **) * GLYPH_DIM);
            for (int j = 0; j < GLYPH_DIM; j++)
            {
                cosalphalookup[i][j] = (double **)malloc(sizeof(double *) * GLYPH_DIM);
                for (int k = 0; k < GLYPH_DIM; k++)
                {
                    cosalphalookup[i][j][k] = (double *)malloc(sizeof(double) * GLYPH_DIM);
                }
            }
            
        }
        
        for(int u = 0; u < GLYPH_DIM; u++)
        {
            for(int v = 0; v < GLYPH_DIM; v++)
            {
                for(int i = 0; i < GLYPH_DIM; i++)
                {
                    for(int j = 0; j < GLYPH_DIM; j++)
                    {
                        cosalphalookup[u][v][i][j] =  alphalookup[i][j]*
                        cos(((M_PI*u)/(2*GLYPH_DIM))*(2*i + 1))*
                        cos(((M_PI*v)/(2*GLYPH_DIM))*(2*j + 1));
                    }
                }
            }
        }
        
        _dctCoeffCheckLimit = 0;
        dctWeights = (double **)malloc(sizeof(double *) * GLYPH_DIM);
        for (int u = 0; u < GLYPH_DIM; u++)
        {
            dctWeights[u] = (double *)malloc(sizeof(double) * GLYPH_DIM);
            
            for (int v = 0; v < GLYPH_DIM; v++)
            {
                double dist = sqrt((double)(u*u + v*v));
                double weight = sqrt(pow(WEIGHT_COEFF, dist));
                dctWeights[u][v] = weight;
                
                if (weight > DCT_WEIGHT_LIMIT)
                {
                    _dctCoeffCheckLimit++;
                }
            }
        }
        
        _sortedGlyphDCValues = (double *)malloc(sizeof(double) * 256);
        _sortedGlyphIndices = (int *)malloc(sizeof(int) * 256);
        
        //glyphs = (double *)malloc(sizeof(double) * 255 * 64);
        glyphs = (unsigned char *)malloc(sizeof(unsigned char) * 255 * 64);
        
        [self prepareGlyphSignatures];
    }
    return self;
}

- (double)alpha:(double)e
{
    if (e == 0)
    {
        return 1.0/sqrt(2.0);
    }
    else
    {
        return 1.0;
    }
}

- (double)getDctDiffBetween:(double *)inputA and:(double *)inputB
{
    double score, diff;
    score = 0;
    
    for (int i = 0; i < _dctCoeffCheckLimit; i++)
    {
        diff = (inputA[i]-inputB[i]);
        diff = diff*diff;
        score += diff;
    }
    
    return score;
}

- (void)dctSubsectionFromBuffer:(const unsigned char *)buffer xstart:(int)xstart y:(int)ystart width:(int)width output:(double *)output
{
    double result;
    int outputindex = 0;
    
    //printf("buffer[1000] = %d\n", buffer[1000]);
    
    for (int i = 0; i < 8; i++)
    {
        // get row addresses of subsection pixels
        subsection[i] = &buffer[(ystart+i)*width + xstart];
    }
    
    // first do 1d DCT on columns
    for (int x = 0; x < GLYPH_DIM; x++)
    {
        for (int v = 0; v < GLYPH_DIM; v++)
        {
            result = 0;
            for (int j = 0; j < GLYPH_DIM; j++)
            {
                result += cosalphalookup1d[v][j] * (double)subsection[j][x];
            }
            dctVert[x][v] = result;
        }
    }
    
    // now do 1d DCT on rows
    for (int y = 0; y < GLYPH_DIM; y++)
    {
        for (int u = 0; u < GLYPH_DIM; u++)
        {
            result = 0;
            if (dctWeights[u][y] > DCT_WEIGHT_LIMIT)
            {
                for (int i = 0; i < GLYPH_DIM; i++)
                {
                    result += cosalphalookup1d[u][i] * dctVert[i][y];
                }
                output[outputindex] = result*dctWeights[u][y];
                outputindex++;
            }
        }
    }
}

- (void)dctWithInput:(double **)input andOutput:(double *)output
{
    double result;
    int outputindex = 0;
    
    // first do 1d DCT on columns
    for (int x = 0; x < GLYPH_DIM; x++)
    {
        for (int v = 0; v < GLYPH_DIM; v++)
        {
            result = 0;
            for (int j = 0; j < GLYPH_DIM; j++)
            {
                result += cosalphalookup1d[v][j] * input[x][j];
            }
            dctVert[x][v] = result;
        }
    }
    
    // now do 1d DCT on rows
    for (int y = 0; y < GLYPH_DIM; y++)
    {
        for (int u = 0; u < GLYPH_DIM; u++)
        {
            result = 0;
            if (dctWeights[u][y] > DCT_WEIGHT_LIMIT)
            {
                for (int i = 0; i < GLYPH_DIM; i++)
                {
                    result += cosalphalookup1d[u][i] * dctVert[i][y];
                }
                output[outputindex] = result*dctWeights[u][y];
                outputindex++;
            }
        }
    }
}



- (void)prepareGlyphSignatures
{
    NSString *glyphString;
    NSString *thisGlyph;
    
    int glyphIndex = 0;
    for (int ch = 0; ch < 256; ch++)
    {
        
        if (ch < 128)
        {
            thisGlyph = [NSString stringWithFormat:@"%d",ch];
        }
        else
        {
            thisGlyph = [NSString stringWithFormat:@"%d",ch-128];
        }
        
        //NSLog(@"%@",thisGlyph);
        glyphString = [NSString stringWithContentsOfFile:[[NSBundle mainBundle] pathForResource:thisGlyph
                                                                                         ofType:@"txt"]
                                                encoding:NSUTF8StringEncoding
                                                   error:nil];
        
        int index = 0;
        unsigned char bit;
        int stride = 8;
        int mult = stride/GLYPH_DIM;
        
        
        for (int y = 0; y < GLYPH_DIM; y++)
        {
            for (int x = 0; x < GLYPH_DIM; x++)
            {
                int yy = y*mult;
                int xx = x*mult;
                
                double sum = 0.0;
                for (int ya = yy; ya < yy+mult; ya++)
                {
                    for (int xa = xx; xa < xx+mult; xa++)
                    {
                        index = ya*stride + xa;
                        bit = [glyphString characterAtIndex:index];
                        
                        if (bit == '0')
                        {
                            if (ch < 128)
                                sum += 0;
                            else
                                sum += 255;
                        }
                        else
                        {
                            if (ch < 128)
                                sum += 255;
                            else
                                sum += 0;
                        }
                    }
                }
                sum /= (mult*mult);
                
                dctInput[x][y] = sum;
                
                glyphs[glyphIndex] = (unsigned char)sum;
                glyphIndex++;
            }
        }
        
        [self dctWithInput:dctInput andOutput:dctSignatures[ch]];
    }
    
    // sort the glyph signatures
    NSMutableArray *signatures = [[NSMutableArray alloc] init];
    for (int i = 0; i < 256; i++)
    {
        NSArray *entry = [NSArray arrayWithObjects:[NSNumber numberWithDouble:dctSignatures[i][0]],
                          [NSNumber numberWithInt:i],
                          nil];
        [signatures addObject:entry];
    }
    
    NSArray *sortedSignatures = [signatures sortedArrayUsingComparator:^NSComparisonResult(id obj1, id obj2)
                                 {
                                     NSArray *firstArr = (NSArray *)obj1;
                                     NSArray *secondArr = (NSArray *)obj2;
                                     
                                     NSNumber *first = [firstArr objectAtIndex:0];
                                     NSNumber *second = [secondArr objectAtIndex:0];
                                     
                                     if ([first doubleValue] < [second doubleValue])
                                     {
                                         return (NSComparisonResult)NSOrderedAscending;
                                     }
                                     else if ([first doubleValue] > [second doubleValue])
                                     {
                                         return (NSComparisonResult)NSOrderedDescending;
                                     }
                                     return (NSComparisonResult)NSOrderedSame;
                                 }];
    
    int ii = 0;
    for (NSArray *arr in sortedSignatures)
    {
        NSNumber *dc = (NSNumber *)[arr objectAtIndex:0];
        NSNumber *ind = (NSNumber *)[arr objectAtIndex:1];
        double dcval = [dc doubleValue];
        int i = [ind intValue];
        
        //NSLog(@"glyph dc %f %d",dcval, i);
        printf("glyph dc %f %d\n",dcval, i);
        
        _sortedGlyphDCValues[ii] = dcval;
        _sortedGlyphIndices[ii] = i;
        
        ii++;
    }
    
    self.sortedGlyphSignatures = sortedSignatures;
    
    NSLog(@"done sorting");
}

- (int)getMatchingGlyph:(double *)dctSearch
{
    double lowest = DBL_MAX;
    double curr_score;
    int matchIndex = 0;
    
    double dc = dctSearch[0];
    double dc_low = dc - 3000.0;
    double dc_high = dc + 3000.0;
    double currdc;
    
    for (int d = 0; d < 256; d++)
    {
        currdc = _sortedGlyphDCValues[d];
        
        if (currdc > dc_low)
        {
            if (currdc > dc_high)
            {
                d = 256;
            }
            else
            {
                int dd = _sortedGlyphIndices[d];
                
                //if (dd != 28 && dd != 156)
                // hack for c64
                {
                    curr_score = [self getDctDiffBetween:dctSearch and:dctSignatures[dd]];
                    if (curr_score < lowest)
                    {
                        matchIndex = dd;
                        lowest = curr_score;
                    }
                }
            }
        }
    }
    
    return matchIndex;
}

- (const unsigned char *)getGlyphAtIndex:(int)index
{
    int glyphIndex = index*64;
    return &glyphs[glyphIndex];
}

@end
