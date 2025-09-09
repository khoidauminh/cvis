#ifndef CVIS_COMMON_H
#define CVIS_COMMON_H

#include <SDL3/SDL_stdinc.h>
#include <complex.h>

typedef float complex cplx;

typedef unsigned long ulong;
typedef unsigned int uint;

typedef struct int2d {
    int x;
    int y;
} Int2D;

typedef struct uint2d {
    uint x;
    uint y;
} Uint2D;

uint uint_min(uint a, uint b);
uint uint_max(uint a, uint b);
int int_max(int a, int b);

cplx quad1(cplx x);
float l1norm(cplx x);
float clampf(float, float, float);
float cmaxf(cplx x);

#endif
