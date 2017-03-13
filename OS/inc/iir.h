#include "config.h"

float *binomial_mult( int n, float *p );

float *dcof_bwlp( int n, float fcf );
float *dcof_bwhp( int n, float fcf );
void dcof_bwbp( int n, float f1f, float f2f, float dcof[]);
void dcof_bwbs( int n, float f1f, float f2f, float dcof[] );

void ccof_bwlp( int n, int ccof[] );
void ccof_bwhp( int n, int ccof[] );
void ccof_bwbp( int n, int ccof[] );
float *ccof_bwbs( int n, float f1f, float f2f );

float sf_bwlp( int n, float fcf );
float sf_bwhp( int n, float fcf );
float sf_bwbp( int n, float f1f, float f2f );
float sf_bwbs( int n, float f1f, float f2f );
