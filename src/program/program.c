#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <assert.h>
#include <ncurses.h>
#include <stdlib.h>
#include <threads.h>

#include "config.h"
#include "logging.h"
#include "program.h"
#include "render.h"

void pg_eventloop_sdl(Program *p);

void pg_eventloop_term(Program *p);

#include "visualizer.h"

VisManager *vm_new(const char *);
void vm_end(VisManager *v);

SDLRenderer *sdl_renderer_new(Program *prog);
void sdl_renderer_end(SDLRenderer *sdlr);

struct program {
    void *renderer;
    VisManager *vismanager;

    // Events:
    KeyEvent keymap[keyevent_null];

    Config cfg;
    void (*eventloop_func)(Program *);
};

Program *pg_new(Config config) {
    Program *p = calloc(1, sizeof(*p));
    assert(p);

    p->cfg = config;
    p->renderer = sdl_renderer_new(p);
    p->vismanager = vm_new(config.visname);
    p->eventloop_func = &pg_eventloop_sdl;

    memset(&p->keymap, 0, sizeof(p->keymap));

    return p;
}

void pg_keymap_set(Program *p, KeyEvent k, bool b) { p->keymap[k] = b; }
bool pg_keymap_get(Program *p, KeyEvent k) { return p->keymap[k]; }

void pg_keymap_reset(Program *p) {
    for (uint i = 0; i < keyevent_null; i++) {
        p->keymap[i] = false;
    }
}

void pg_keymap_print(Program *p) {
    for (uint i = 0; i < keyevent_null; i++) {
        info("%d", p->keymap[i]);
    }
    info("\n");
}

SDLRenderer *pg_renderer(Program *p) { return p->renderer; }

Config *pg_config(Program *p) { return &p->cfg; }

void pg_eventloop(Program *p) { (p->eventloop_func)(p); }

VisManager *pg_vismanager(Program *p) { return p->vismanager; }

void pg_end(Program *p) {
    sdl_renderer_end(p->renderer);
    vm_end(p->vismanager);
    free(p);
}

static Program *STATIC_PG(Program *p) {
    static Program *PROGRAM = nullptr;

    if (PROGRAM == nullptr) {
        assert(p != nullptr);
        PROGRAM = p;
    }

    return PROGRAM;
}

void PG_SET_TARGET(Program *p) { STATIC_PG(p); }

Program *PG_GET() { return STATIC_PG(nullptr); }

SDLRenderer *PG_RENDERER() { return PG_GET()->renderer; }

const Config *PG_CONFIG() { return &PG_GET()->cfg; }

const VisManager *PG_VISMANAGER() { return PG_GET()->vismanager; }
