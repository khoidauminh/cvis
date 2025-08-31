#include "common.h"

#include <math.h>

cplx quad1(cplx x) { return CMPLX(fabsf(crealf(x)), fabsf(cimagf(x))); }
float l1norm(cplx x) { return fabsf(crealf(x)) + fabsf(cimagf(x)); }
float cmaxf(cplx x) { return fmaxf(fabsf(crealf(x)), fabsf(cimagf(x))); }

float clampf(float min, float x, float max) {
    return fmaxf(min, fminf(x, max));
}

uint uint_min(uint a, uint b) { return (a < b) ? a : b; }
uint uint_max(uint a, uint b) { return (a > b) ? a : b; }

int int_max(int a, int b) { return (a > b) ? a : b; }
