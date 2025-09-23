#ifndef CVIS_INTERPOLATION_H
#define CVIS_INTERPOLATION_H

#include "common.h"

float linearf(float a, float b, float t);
tCplx clinearf(tCplx a, tCplx b, float t);

float decay(float prev, float now, float factor);
float linear_decay(float, float, float);
float smooth_step(float a, float b, float t);

#endif
