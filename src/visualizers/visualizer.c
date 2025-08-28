#include "visualizer.h"

#include <assert.h>
#include <stdlib.h>

struct visualizer_manager {
    Visualizer **data;
    Visualizer **func;
};

static Visualizer *FUNC_ARRAY[] = {&visualizer_spectrum, &visualizer_spectrum,
                                   NULL};

VisManager *vm_new() {
    VisManager *v = malloc(sizeof(VisManager));
    assert(v);

    v->data = FUNC_ARRAY;
    v->func = v->data;

    return v;
}

void vm_next(VisManager *v) {
    v->func++;
    if (!*v->func)
        v->func = v->data;
}

Visualizer *vm_current(VisManager *v) { return *v->func; }

void vm_end(VisManager *v) { free(v); }
