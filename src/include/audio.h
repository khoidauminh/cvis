#ifndef AUDIO_H
#define AUDIO_H

#include "declare.h"

uint buffer_read(cplx*, uint);
void buffer_autoslide();
cplx *buffer_get(uint index);

void init_audio();
void free_audio();

void normalize_average(cplx*, uint);
void normalize_max(cplx*, uint, float);
void slow_regain(cplx *samples, uint len, float gain, float t);
void compress(cplx *samples, uint len, float limit, float gain);

#endif
