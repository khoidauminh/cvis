#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <assert.h>
#include <stdlib.h>

#include "common.h"
#include "config.h"
#include "logging.h"
#include "render.h"

#include "render-private.h" // IWYU pragma: keep.

static void check_vtable_print_error(bool *is_error) {
    if (!*is_error) {
        error("Missing functions in the vtable:\n");
    }

    *is_error = true;
}

static void check_vtable(const RenderVTable *const vtable) {

    if (!vtable) {
        die("VTable missing!");
    }

    bool error = false;

#define CHECK(field)                                                           \
    if (!vtable->field) {                                                      \
        check_vtable_print_error(&error);                                      \
        info("-- %s\n", #field);                                               \
    }

    CHECK(plot);
    CHECK(rect);
    CHECK(color);
    CHECK(resize);
    CHECK(blend);
    CHECK(fill);
    CHECK(clear);
    CHECK(flush);

    if (error) {
        abort();
    }

#undef CHECK
}

Renderer *renderer_new(Config *cfg) {
    Renderer *out = malloc(sizeof(Renderer));
    assert(out);

    *out = (Renderer){
        .type = displaymode_get_renderer(cfg->displaymode),
        .cfg = cfg,
    };

    switch (out->type) {
    case renderertype_sdl:
        out->init = sdl_renderer_init;
        out->exit = sdl_renderer_end;
        break;

    case renderertype_terminal:
        out->init = terminal_renderer_init;
        out->exit = terminal_renderer_end;
        break;
    default:
        die("Invalid renderer type.");
    }

    (out->init)(out);

    check_vtable(out->vtable);

    return out;
}

void renderer_end(Renderer *r) {
    (r->exit)(r);
    free(r);
}

RendererType renderer_get_type(Renderer *r) { return r->type; }

static Renderer *RENDERER = nullptr;

void RNDR_SET_TARGET(Renderer *r) {
    assert(r);
    RENDERER = r;
}

Uint2D RNDR_SIZE() {
    return (Uint2D){.x = RENDERER->cfg->width, .y = RENDERER->cfg->height};
}

void RNDR_COLOR(Color c) { (RENDERER->vtable->color)(RENDERER, c); }

void RNDR_PLOT(float x, float y) { (RENDERER->vtable->plot)(RENDERER, x, y); }

void RNDR_RECT(float x, float y, float w, float h) {
    (RENDERER->vtable->rect)(RENDERER, x, y, w, h);
}

void RNDR_LINE(float x1, float y1, float x2, float y2) {
    (RENDERER->vtable->line)(RENDERER, x1, y1, x2, y2);
}

void RNDR_FADE(Uint8 a) { (RENDERER->vtable->fade)(RENDERER, a); }

void RNDR_FILL() { (RENDERER->vtable->fill)(RENDERER); }

void RNDR_CLEAR() { (RENDERER->vtable->clear)(RENDERER); }

void RNDR_BLEND(SDL_BlendMode blendmode) {
    (RENDERER->vtable->blend)(RENDERER, blendmode);
}

void RNDR_FLUSH() {
    (RENDERER->vtable->flush)(RENDERER);
    RNDR_BLEND(SDL_BLENDMODE_NONE);
}

void RNDR_TEXT(float x, float y, const char *str, TextAlignment align,
               TextAnchor anchor) {
    (RENDERER->vtable->text)(RENDERER, x, y, str, align, anchor);
}

void RNDR_AUTORESIZE() { (RENDERER->vtable->resize)(RENDERER); }
