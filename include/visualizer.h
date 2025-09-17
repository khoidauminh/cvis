#ifndef CVIS_INTERNAL_VISUALIZER_H
#define CVIS_INTERNAL_VISUALIZER_H

typedef struct visualizer_manager VisManager;

typedef void(VisFunc)();

#include "public/program.h"

VisManager *vm_new(const char *);
void vm_end(VisManager *v);

void vm_perform(Program *);
void vm_next(VisManager *);

#endif
