#ifndef CVIS_RENDER_PRIVATE_H
#define CVIS_RENDER_PRIVATE_H

#include "common.h"
#include "render.h"
#include <SDL3/SDL_stdinc.h>

typedef struct render_vtable {
    void (*plot)(Renderer *, float x, float y);
    void (*rect)(Renderer *, float x, float y, float w, float h);
    void (*color)(Renderer *, Color c);
    void (*resize)(Renderer *);
    void (*line)(Renderer *, float x1, float y1, float x2, float y2);
    void (*blend)(Renderer *, uint);
    void (*fill)(Renderer *);
    void (*clear)(Renderer *);
    void (*fade)(Renderer *, Uint8 a);
    void (*flush)(Renderer *);
    void (*text)(Renderer *, float x, float y, const char *str,
                 TextAlignment align, TextAnchor anchor);
} RenderVTable;

#include "config.h"

struct renderer {
    RendererType type;
    void *renderer;

    void (*init)(Renderer *);
    void (*exit)(Renderer *);

    Config *cfg;
    const RenderVTable *vtable;
};

#include "program.h"

// PRIVATE HEADERS:
void RNDR_SET_TARGET(Renderer *r);
void PG_SET_TARGET(Program *p);
void RNDR_FLUSH();
void RNDR_AUTORESIZE();
void vm_perform(Program *);
void vm_next(VisManager *);

RendererType renderer_get_type(Renderer *r);

void sdl_renderer_init(Renderer *r);
void sdl_renderer_end(Renderer *r);

void terminal_renderer_init(Renderer *r);
void terminal_renderer_end(Renderer *r);

#endif
