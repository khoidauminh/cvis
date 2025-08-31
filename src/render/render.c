#include <SDL3/SDL_pixels.h>
#include <assert.h>
#include <stdlib.h>

#include "common.h"
#include "config.h"
#include "logging.h"
#include "render.h"

#include "renderer-private.h" // IWYU pragma: keep.

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
#ifdef USE_RAYLIB
    case renderertype_raylib:
        out->init = raylib_init;
        out->exit = raylib_end;
        break;
#endif
    case renderertype_terminal:
        out->init = terminal_renderer_init;
        out->exit = terminal_renderer_end;
        break;
    default:
        die("Invalid renderer type.");
    }

    (out->init)(out);

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
    APIParameter c = {.color = {
                          .r = r,
                          .g = g,
                          .b = b,
                          .a = a,
                      }};
    (renderer->api[renderapi_color])(renderer, &c);
}

void render_plot(Renderer *r, float x, float y) {
    APIParameter p = {.plot = {x, y}};
    (r->api[renderapi_plot])(r, &p);
}

void render_rect(Renderer *r, float x, float y, float w, float h) {
    APIParameter p = {.rect = {x, y, w, h}};
    (r->api[renderapi_rect])(r, &p);
}

void render_fill(Renderer *r) { (r->api[renderapi_fill])(r, nullptr); }

void render_flush(Renderer *r) { (r->api[renderapi_flush])(r, nullptr); }

void render_clear(Renderer *r) { (r->api[renderapi_clear])(r, nullptr); }

void render_autoresize(Renderer *r) { (r->api[renderapi_resize])(r, nullptr); }

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

void RNDR_FLUSH() { render_flush(RENDERER); }

void RNDR_AUTORESIZE() { render_autoresize(RENDERER); }
