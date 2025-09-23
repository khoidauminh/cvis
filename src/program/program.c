#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <assert.h>
#include <stdlib.h>
#include <threads.h>

#include "public/config.h"
#include "public/logging.h"

#include "program.h"
#include "render.h"
#include "visualizer.h"

struct program {
    sRenderer *renderer;
    sVisManager *vismanager;

    // Events:
    eKeyEvent keymap[KEYEVENT_NULL];

    sConfig cfg;
    void (*eventloop_func)(sProgram *);
};

sProgram *pg_new(sConfig config) {
    sProgram *p = calloc(1, sizeof(*p));
    assert(p);

    p->cfg = config;
    p->renderer = sdl_renderer_new(p);
    p->vismanager = vm_new(config.visname);
    p->eventloop_func = &pg_eventloop_sdl;

    memset(&p->keymap, 0, sizeof(p->keymap));

    return p;
}

void pg_keymap_set(sProgram *p, eKeyEvent k, bool b) { p->keymap[k] = b; }
bool pg_keymap_get(sProgram *p, eKeyEvent k) { return p->keymap[k]; }

void pg_keymap_reset(sProgram *p) {
    for (tUint i = 0; i < KEYEVENT_NULL; i++) {
        p->keymap[i] = false;
    }
}

void pg_keymap_print(sProgram *p) {
    for (tUint i = 0; i < KEYEVENT_NULL; i++) {
        info("%d", p->keymap[i]);
    }
    info("\n");
}

sRenderer *pg_renderer(sProgram *p) { return p->renderer; }

sConfig *pg_config(sProgram *p) { return &p->cfg; }

void pg_eventloop(sProgram *p) { (p->eventloop_func)(p); }

sVisManager *pg_vismanager(sProgram *p) { return p->vismanager; }

void pg_end(sProgram *p) {
    sdl_renderer_end(p->renderer);
    vm_end(p->vismanager);
    free(p);
}

static sProgram *STATIC_PG(sProgram *p) {
    static sProgram *PROGRAM = nullptr;

    if (PROGRAM == nullptr) {
        assert(p != nullptr);
        PROGRAM = p;
    }

    return PROGRAM;
}

void PG_SET_TARGET(sProgram *p) { STATIC_PG(p); }

sProgram *PG_GET() { return STATIC_PG(nullptr); }

sRenderer *PG_RENDERER() { return PG_GET()->renderer; }

sConfig *PG_CONFIG() { return &PG_GET()->cfg; }

bool PG_KEYPRESSED(eKeyEvent k) { return pg_keymap_get(PG_GET(), k); }

const sVisManager *PG_VISMANAGER() { return PG_GET()->vismanager; }
