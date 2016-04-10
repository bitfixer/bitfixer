//
//  DCT.h
//  soundcontroller
//
//  Created by Michael Hill on 4/9/16.
//  Copyright (c) 2016 Michael Hill. All rights reserved.
//

#ifndef __soundcontroller__DCT__
#define __soundcontroller__DCT__

#include <stdlib.h>

class DCT
{
public:
    DCT(int length);
    ~DCT();
    void getDCT(float *input, float *output, int size);
    float delta(int i);
    
private:
    int length;
    float **coslookup = NULL;
};

#endif /* defined(__soundcontroller__DCT__) */
