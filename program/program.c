#include <SDL3/SDL_timer.h>
#include <assert.h>
#include <ncurses.h>
#include <stdlib.h>

#include "config.h"
#include "logging.h"
#include "program.h"
#include "render.h"

#include "rl.h"
#include "sdl.h"
#include "terminal.h"

#include "visualizer.h"

struct program {
    Renderer *renderer;
    VisManager *vismanager;
    Config cfg;
};

Program *pg_new(Config config) {
    Program *p = calloc(1, sizeof(*p));
    assert(p);

    p->cfg = config;
    p->renderer = renderer_new(&p->cfg);
    p->vismanager = vm_new(config.visname);

    return p;
}

Config *pg_config(Program *p) { return &p->cfg; }

void pg_eventloop(Program *p) {
    switch (p->cfg.displaymode) {
    case displaymode_graphical:
#ifdef USE_RAYLIB
        pg_eventloop_raylib(p);
#else
        pg_eventloop_sdl(p);
#endif
        break;
    case displaymode_terminal:
        pg_eventloop_term(p);
        break;
    default:
        die("Invalid displaymode value! It might be corrupt.");
    }
}

Renderer *pg_renderer(Program *p) { return p->renderer; }
VisManager *pg_vismanager(Program *p) { return p->vismanager; }
void pg_attach_renderer(Program *p, Renderer *r) { p->renderer = r; }

void pg_end(Program *p) {
    renderer_end(p->renderer);
    vm_end(p->vismanager);
    free(p);
}
