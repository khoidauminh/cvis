#include "public/interpolation.h"
#include "public/common.h"

#include <math.h>

float linearf(float a, float b, float t) { return a + (b - a) * t; }
tCplx clinearf(tCplx a, tCplx b, float t) { return a + (b - a) * t; }

float linear_decay(float prev, float now, float step) {
    return fmaxf(now, prev - step);
}

float decay(float prev, float now, float factor) {
    return fmaxf(now, prev * factor);
}

float smooth_step(float a, float b, float t) {
    t = t - 0.5f;
    t = t * (2.0f - 2.0f * fabsf(t)) + 0.5f;
    return a + (b - a) * t;
}
