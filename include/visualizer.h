#ifndef CVIS_INTERNAL_VISUALIZER_H
#define CVIS_INTERNAL_VISUALIZER_H

typedef struct visualizer_manager sVisManager;

typedef void(fVisFunc)();

#include "public/program.h"

sVisManager *vm_new(const char *);
void vm_end(sVisManager *v);

void vm_perform(sProgram *);
void vm_next(sVisManager *);

#endif
