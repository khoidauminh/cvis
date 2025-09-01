#ifndef VISUALIZER_C
#define VISUALIZER_C

typedef struct visualizer_manager VisManager;

#include "program.h"

typedef void(VisFunc)(Program *);

VisManager *vm_new(const char*);
void vm_next(VisManager *v);
void vm_end(VisManager *v);

VisFunc *vm_current(VisManager *v);
void vm_perform(Program *p);
void vm_selfupdate(VisManager*);

void visualizer_spectrum(Program *);
void visualizer_vectorscope(Program *);
void visualizer_oscilloscope(Program *prog);

#endif
