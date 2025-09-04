#include <SDL3/SDL_pixels.h>
#include <assert.h>
#include <stdlib.h>

#include "common.h"
#include "config.h"
#include "logging.h"
#include "render.h"

#include "renderer-private.h" // IWYU pragma: keep.

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

uint renderer_get_width(Renderer *r) { return r->cfg->width; }

uint renderer_get_height(Renderer *r) { return r->cfg->height; }

Size renderer_get_size(Renderer *r) {
    return (Size){.w = r->cfg->width, .h = r->cfg->height};
}

void render_set_color(Renderer *renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    (renderer->vtable->color)(renderer, r, g, b, a);
}

void render_plot(Renderer *r, float x, float y) { (r->vtable->plot)(r, x, y); }

void render_rect(Renderer *r, float x, float y, float w, float h) {
    (r->vtable->rect)(r, x, y, w, h);
}

void render_set_blendmode(Renderer *r, SDL_BlendMode blendmode) {
    (r->vtable->blend)(r, blendmode);
}

void render_flush(Renderer *r) { (r->vtable->flush)(r); }

void render_fill(Renderer *r) { (r->vtable->clear)(r); }

void render_clear(Renderer *r) { (r->vtable->clear)(r); }

void render_autoresize(Renderer *r) { (r->vtable->resize)(r); }

static Renderer *RENDERER = nullptr;

void RNDR_SET_TARGET(Renderer *r) {
    assert(r);
    RENDERER = r;
}

Size RNDR_SIZE() { return renderer_get_size(RENDERER); }

void RNDR_COLOR(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    render_set_color(RENDERER, r, g, b, a);
}

void RNDR_PLOT(float x, float y) { render_plot(RENDERER, x, y); }

void RNDR_RECT(float x, float y, float w, float h) {
    render_rect(RENDERER, x, y, w, h);
}

void RNDR_FILL() { render_fill(RENDERER); }

void RNDR_CLEAR() { render_clear(RENDERER); }

void RNDR_FLUSH() {
    render_flush(RENDERER);
    render_set_blendmode(RENDERER, SDL_BLENDMODE_NONE);
}

void RNDR_BLEND(SDL_BlendMode blendmode) {
    render_set_blendmode(RENDERER, blendmode);
}

void RNDR_AUTORESIZE() { render_autoresize(RENDERER); }
