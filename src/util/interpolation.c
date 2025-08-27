#include "interpolation.h"
#include "declare.h"

#define GEN_LINEAR(a, b, t) (a + (b - a) * t)

float linearf(float a, float b, float t) { return GEN_LINEAR(a, b, t); }
cplx clinearf(cplx a, cplx b, float t) { return GEN_LINEAR(a, b, t); }

float decay(float prev, float now, float factor) {
    if (now > prev) {
        return now;
    }

    float new = prev * factor;

    if (new < 0.0) {
        return 0.0;
    }

    if (new < now) {
        return now;
    }

    return new;
}
