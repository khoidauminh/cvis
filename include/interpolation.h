#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include "common.h"

float linearf(float a, float b, float t);
cplx clinearf(cplx a, cplx b, float t);

float decay(float prev, float now, float factor);
float linear_decay(float, float, float);
float smooth_step(float a, float b, float t);

#endif
