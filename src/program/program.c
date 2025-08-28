#include <assert.h>
#include <stdlib.h>

#include "declare.h"
#include "render.h"

#include "program.h"

#include "visualizer.h"

struct program {
    Renderer *renderer;
    VisManager *vismanager;
    uint refreshrate;
};

Program *pg_new(RendererType type, uint rr) {
    Program *p = calloc(1, sizeof(*p));
    assert(p);

    p->renderer = renderer_new(type, DEFAULT_WIN_SIZE, DEFAULT_WIN_SIZE);
    p->vismanager = vm_new();
    p->refreshrate = rr;

    return p;
}

void pg_eventloop(Program *p) {

    bool running = true;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_SPACE) {
                    vm_next(pg_vismanager(p));
                }

                break;
            default: {
            }
            }
        }

        (vm_current(pg_vismanager(p)))(p);
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
