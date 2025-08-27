#ifndef AUDIO_H
#define AUDIO_H

#include "declare.h"

uint buffer_read(void*, uint);
void buffer_slide(uint);
cplx *buffer_get(uint index);

void init_audio();
void free_audio();

void normalize_average(cplx*, uint);
void normalize_max(cplx*, uint);

#endif
