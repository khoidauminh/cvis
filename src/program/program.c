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

Renderer *renderer_new(Config *cfg);
void renderer_end(Renderer *);

VisManager *vm_new(const char *);
void vm_end(VisManager *v);

struct program {
    Renderer *renderer;
    VisManager *vismanager;
    KeyEvent keymap[keyevent_null];
    Config cfg;
    void (*eventloop_func)(Program *);
};

Program *pg_new(Config config) {
    Program *p = calloc(1, sizeof(*p));
    assert(p);

    p->cfg = config;
    p->renderer = renderer_new(&p->cfg);
    p->vismanager = vm_new(config.visname);
    memset(&p->keymap, 0, sizeof(p->keymap));

    switch (p->cfg.displaymode) {
    case CVIS_DISPLAYMODE_GRAPHICAL:
        p->eventloop_func = &pg_eventloop_sdl;
        break;
    case CVIS_DISPLAYMODE_TERMINAL:
        p->eventloop_func = &pg_eventloop_term;
        break;
    default:
        die("Invalid displaymode value! It might be corrupt.");
    }

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

Config *pg_config(Program *p) { return &p->cfg; }

void pg_eventloop(Program *p) { (p->eventloop_func)(p); }

Renderer *pg_renderer(Program *p) { return p->renderer; }
VisManager *pg_vismanager(Program *p) { return p->vismanager; }
void pg_attach_renderer(Program *p, Renderer *r) { p->renderer = r; }

void pg_end(Program *p) {
    renderer_end(p->renderer);
    vm_end(p->vismanager);
    free(p);
}

static thread_local Program *PROGRAM = nullptr;

void PG_SET_TARGET(Program *p) {
    assert(p);
    PROGRAM = p;
}

Program *PG_GET() {
    assert(PROGRAM);
    return PROGRAM;
}

const Config *PG_CONFIG() { return &PG_GET()->cfg; }

const VisManager *PG_VISMANAGER() { return PG_GET()->vismanager; }
