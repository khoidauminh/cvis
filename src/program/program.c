#include <assert.h>
#include <stdlib.h>

#include "declare.h"
#include "render.h"

#include "program.h"

Program *program_new(RendererType, uint rr) {
    Program *p = calloc(1, sizeof(*p));
    assert(p);

    p->refreshrate = rr;

    return p;
}
