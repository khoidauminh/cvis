#ifndef CVIS_AUDIO_H
#define CVIS_AUDIO_H

#include "common.h"

constexpr tUint SAMPLERATE = 48000;

void cplxcpy(tCplx *restrict dst, const tCplx *restrict src, tUint amount);
void cplxzero(tCplx *restrict buf, tUint amount);

tUint BUFFER_READ(tCplx *, tUint);
void BUFFER_SLIDE(const tUint amount);
void BUFFER_AUTOSLIDE();
tCplx BUFFER_GET(tUint index);
tUint BUFFER_INPUTSIZE();
tUlong BUFFER_AGE();
float BUFFER_CURRENT_PEAK();
bool BUFFER_QUIET();

void normalize_average(tCplx *, tUint);
void normalize_max(tCplx *, tUint, float);
void slow_regain(tCplx *samples, tUint len, float gain, float t);
void compress(tCplx *samples, tUint len, float lo, float hi);

#endif
