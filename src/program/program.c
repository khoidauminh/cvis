#include <assert.h>
#include <stdlib.h>

#include "declare.h"
#include "render.h"

#include "program.h"

struct program {
    Renderer *renderer;
    uint refreshrate;
};

Program *pg_new(RendererType, uint rr) {
    Program *p = calloc(1, sizeof(*p));
    assert(p);

    p->refreshrate = rr;

    return p;
}

Renderer *pg_renderer(Program *p) { return p->renderer; }
void pg_attach_renderer(Program *p, Renderer *r) { p->renderer = r; }

void pg_end(Program *p) { free(p); }
