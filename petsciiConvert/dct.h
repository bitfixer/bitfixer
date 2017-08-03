//
//  dct.h
//  petsciiConvert2
//
//  Created by Michael Hill on 9/18/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef petsciiConvert2_dct_h
#define petsciiConvert2_dct_h

double alpha(double e);
void generateAlphaLookup(double **alphaLookup, int dim);
void generateCosLookup(double ****cosLookup, double **alphaLookup, int height);
void generateCos1DLookup(double **cos1DLookup, int dim);
void dctWithInput(double ** input, double *output, double ****cosLookup, int dim);
void dct1WithInput(double ** input, double *output, double **cosLookup, int dim);

#endif
