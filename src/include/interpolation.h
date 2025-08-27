#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include "declare.h"

float linearf(float a, float b, float t);
cplx clinearf(cplx a, cplx b, float t);

float decay(float prev, float now, float factor);

#endif
