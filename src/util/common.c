#include "public/common.h"

#include <math.h>
#include <stdbit.h>
#include <time.h>

tCplx quad1(tCplx x) { return CMPLXF(fabsf(crealf(x)), fabsf(cimagf(x))); }
float l1norm(tCplx x) { return fabsf(crealf(x)) + fabsf(cimagf(x)); }
float cmaxf(tCplx x) { return fmaxf(fabsf(crealf(x)), fabsf(cimagf(x))); }

float clampf(float min, float x, float max) {
    return fmaxf(min, fminf(x, max));
}

tUint ulog2(tUint x) { return stdc_bit_width(x >> 1); }

tUint uint_min(tUint a, tUint b) { return (a < b) ? a : b; }
tUint uint_max(tUint a, tUint b) { return (a > b) ? a : b; }

int int_max(int a, int b) { return (a > b) ? a : b; }

tInstant instant() {
    static thread_local struct timespec ts;
    timespec_get(&ts, TIME_UTC);

    constexpr tInstant DIV = 1000000000 / INSTANT_SECOND;

    tInstant out =
        (tInstant)ts.tv_sec * INSTANT_SECOND + (tInstant)ts.tv_nsec / DIV;

    return out;
}
