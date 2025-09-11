#include "visualizer.h"

#include "logging.h"
#include "program.h"
#include <assert.h>
#include <stdlib.h>
#include <time.h>

constexpr double AUTOSWITCH_ITERVAL = 8.0;

typedef struct visualizer {
    const char *const name;
    VisFunc *const func;
} Visualizer;

struct visualizer_manager {
    const Visualizer *list;
    const Visualizer *ptr;
    time_t instant;
    bool autoswitch;
};

void visualizer_spectrum();
void visualizer_vectorscope();
void visualizer_oscilloscope();
void visualizer_slice();
void game_snake();

static const Visualizer FUNC_ARRAY[] = {
    {.name = "spectrum", .func = visualizer_spectrum},
    {.name = "vectorscope", .func = visualizer_vectorscope},
    {.name = "oscilloscope", .func = visualizer_oscilloscope},
    {.name = "slice", .func = visualizer_slice},

    {.name = "snake", .func = game_snake},

    {}};

void vm_select_by_name(VisManager *v, const char *name);

VisManager *vm_new(const char *name) {
    VisManager *v = malloc(sizeof(VisManager));
    assert(v);

    v->list = FUNC_ARRAY;
    v->ptr = v->list;
    v->instant = time(nullptr);
    v->autoswitch = false;

    if (name && strlen(name) != 0) {
        vm_select_by_name(v, name);
    }

    return v;
}

void vm_select_by_name(VisManager *v, const char *name) {
    for (const Visualizer *i = v->list; i->func; i++) {
        if (!strcmp(i->name, name)) {
            v->ptr = i;
            return;
        }
    }

    warn("Failed to find the specified name.\n");
    info("Available names:\n");

    for (const Visualizer *i = v->list; i->func; i++) {
        info("%s\n", i->name);
    }
}

void vm_next(VisManager *v) {
    v->ptr++;
    if (!v->ptr->func)
        v->ptr = v->list;
}

void vm_selfupdate(VisManager *v) {
    if (v->autoswitch) {
        time_t newinstant = time(nullptr);

        if (difftime(newinstant, v->instant) >= AUTOSWITCH_ITERVAL) {
            v->instant = newinstant;
            vm_next(v);
        }
    }
}

void vm_perform(Program *p) {
    VisManager *vm = pg_vismanager(p);
    vm_selfupdate(vm);

    const Visualizer *vis = vm->ptr;

    (vis->func)();
}

VisFunc *vm_current_func(VisManager *v) { return *v->ptr->func; }

void vm_end(VisManager *v) { free(v); }
