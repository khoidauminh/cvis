#include "public/common.h"

#include <math.h>
#include <stdbit.h>
#include <time.h>

cplx quad1(cplx x) { return CMPLXF(fabsf(crealf(x)), fabsf(cimagf(x))); }
float l1norm(cplx x) { return fabsf(crealf(x)) + fabsf(cimagf(x)); }
float cmaxf(cplx x) { return fmaxf(fabsf(crealf(x)), fabsf(cimagf(x))); }

float clampf(float min, float x, float max) {
    return fmaxf(min, fminf(x, max));
}

uint ulog2(uint x) { return stdc_bit_width(x >> 1); }

uint uint_min(uint a, uint b) { return (a < b) ? a : b; }
uint uint_max(uint a, uint b) { return (a > b) ? a : b; }

int int_max(int a, int b) { return (a > b) ? a : b; }

instant_t instant() {
    static thread_local struct timespec ts;
    timespec_get(&ts, TIME_UTC);

    constexpr instant_t DIV = 1000000000 / INSTANT_SECOND;

    instant_t out =
        (instant_t)ts.tv_sec * INSTANT_SECOND + (instant_t)ts.tv_nsec / DIV;

    return out;
}
