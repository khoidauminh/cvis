#ifndef CVIS_AUDIO_H
#define CVIS_AUDIO_H

#include "common.h"

constexpr uint SAMPLERATE = 44100;

void cplxcpy(cplx *restrict dst, const cplx *restrict src, uint amount);
void cplxzero(cplx *restrict buf, uint amount);

uint BUFFER_READ(cplx*, uint);
void BUFFER_SLIDE(const uint amount);
void BUFFER_AUTOSLIDE();
cplx BUFFER_GET(uint index);

void init_audio();
void free_audio();

void normalize_average(cplx*, uint);
void normalize_max(cplx*, uint, float);
void slow_regain(cplx *samples, uint len, float gain, float t);
void compress(cplx *samples, uint len, float lo, float hi);

#endif
