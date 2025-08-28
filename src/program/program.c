#include <SDL3/SDL_timer.h>
#include <assert.h>
#include <ncurses.h>
#include <stdlib.h>

#include "config.h"
#include "logging.h"
#include "program.h"
#include "render.h"
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

void pg_eventloop_term(Program *p) {
    assert(renderer_get_type(pg_renderer(p)) == renderertype_terminal);

    bool running = true;

    while (running) {
        const char key = getch();

        switch (key) {
        case 'q':
            running = false;
            break;
        case ' ':
            vm_next(pg_vismanager(p));
            break;
        default: {
        }
        }

        render_autoresize(pg_renderer(p));
        vm_perform(p);
        SDL_Delay(1000 / 60);
    }
}

void pg_eventloop_win(Program *p) {
    assert(renderer_get_type(pg_renderer(p)) == renderertype_sdl);

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

        vm_perform(p);

        if (p->cfg.refreshmode == refreshmode_set)
            SDL_Delay(1000 / p->cfg.refreshrate);
    }
}

void pg_eventloop(Program *p) {
    switch (p->cfg.displaymode) {
    case displaymode_graphical:
        pg_eventloop_win(p);
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
