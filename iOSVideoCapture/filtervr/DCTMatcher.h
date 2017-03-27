//
//  DCTMatcher.h
//  testcam
//
//  Created by Michael Hill on 3/28/15.
//
//

#import <Foundation/Foundation.h>

@interface DCTMatcher : NSObject

- (id)init;
- (void)prepareGlyphSignatures;
- (void)dctWithInput:(double **)input andOutput:(double *)output;
- (void)dctSubsectionFromBuffer:(const unsigned char *)buffer xstart:(int)xstart y:(int)ystart width:(int)width output:(double *)output;
- (int)getMatchingGlyph:(double *)dctSearch;
//- (const double *)getGlyphAtIndex:(int)index;
- (const unsigned char *)getGlyphAtIndex:(int)index;

@property (nonatomic, retain) NSArray *sortedGlyphSignatures;

@end
