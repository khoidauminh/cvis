#ifndef DECLARE_H
#define DECLARE_H

#include <complex.h>
#include <SDL3/SDL_stdinc.h>

typedef float complex cplx;
typedef unsigned long uint;
typedef long sint;
typedef struct size { uint w; uint h; } Size;

cplx quad1(cplx x);
float clampf(float, float, float);
float cmaxf(cplx x);

#endif
