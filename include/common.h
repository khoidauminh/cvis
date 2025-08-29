#ifndef COMMON_H
#define COMMON_H

#include <complex.h>
#include <SDL3/SDL_stdinc.h>

typedef float complex cplx;

typedef unsigned int uint;

typedef struct size { uint w; uint h; } Size;

uint uint_min(uint a, uint b);
cplx quad1(cplx x);
float l1norm(cplx x);
float clampf(float, float, float);
float cmaxf(cplx x);

#endif
