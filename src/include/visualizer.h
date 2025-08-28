#ifndef VISUALIZER_C
#define VISUALIZER_C

typedef struct visualizer_manager VisManager;

#include "program.h"

typedef void(Visualizer)(Program *);

VisManager *vm_new();
void vm_next(VisManager *v);
void vm_end(VisManager *v);

Visualizer *vm_current(VisManager *v);
void visualizer_spectrum(Program *);
void visualizer_vectorscope(Program *);

#endif
