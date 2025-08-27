#ifndef AUDIO_H
#define AUDIO_H

#include "declare.h"

void buffer_read(void*, uint);
void buffer_rotate_left(uint);

void init_audio();
void free_audio();

#endif
