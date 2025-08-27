#ifndef VISUALIZER_C
#define VISUALIZER_C

#include "program.h"

typedef void(Visualizer)(Program *);

void visualizer_spectrum(Program *);
void visualizer_vectorscope(Program *);

#endif
