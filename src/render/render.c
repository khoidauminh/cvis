#include <assert.h>
#include <stdlib.h>

#include "declare.h"
#include "logging.h"
#include "render.h"
#include "terminal.h"
#include "window.h"
#include <SDL3/SDL_pixels.h>

#include "renderer-private.h" // IWYU pragma: keep.

Renderer *renderer_init(RendererType type, uint width, uint height) {
    Renderer *out = malloc(sizeof(Renderer));
    assert(out);

    *out = (Renderer){.type = type,
                      .width = width,
                      .height = height,
                      .background =
                          (SDL_Color){.r = 30, .g = 30, .b = 30, .a = 255}};

    switch (type) {
    case rt_sdl:
        window_renderer_init(out);
        break;
    case rt_console:
        terminal_renderer_init(out);
        break;
    default:
        die("Invalid renderer type.");
    }

    return out;
}

void renderer_end(Renderer *r) {
    switch (r->type) {
    case rt_sdl:
        window_renderer_end(r);
        break;
    case rt_console:
        terminal_renderer_end(r);
        break;
    default:
        die("Invalid Value");
    }
    free(r);
}

uint renderer_get_width(Renderer *r) { return r->width; }

uint renderer_get_height(Renderer *r) { return r->height; }

Size renderer_get_size(Renderer *r) {
    return (Size){.w = r->width, .h = r->height};
}

void render_set_color(Renderer *renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    DrawParameter c = {.color = {
                           .r = r,
                           .g = g,
                           .b = b,
                           .a = a,
                       }};
    (renderer->api[dt_color])(renderer, &c);
}

void render_plot(Renderer *r, float x, float y) {
    DrawParameter p = {.plot = {x, y}};
    (r->api[dt_plot])(r, &p);
}

void render_rect_wh(Renderer *r, float x, float y, float w, float h) {
    DrawParameter p = {.rect_wh = {x, y, w, h}};
    (r->api[dt_rect_wh])(r, &p);
}

void render_rect_xy(Renderer *r, float x1, float y1, float x2, float y2) {
    DrawParameter p = {.rect_xy = {x1, y1, x2, y2}};
    (r->api[dt_rect_xy])(r, &p);
}

void render_fill(Renderer *r) { (r->api[dt_fill])(r, NULL); }

void render_flush(Renderer *r) { (r->api[dt_flush])(r, NULL); }

void render_clear(Renderer *r) { (r->api[dt_clear])(r, NULL); }

static Renderer *RENDERER = NULL;

void RNDR_SET_TARGET(Renderer *r) {
    assert(r);
    RENDERER = r;
}

Size RNDR_SIZE() { return renderer_get_size(RENDERER); }

void RNDR_COLOR(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    render_set_color(RENDERER, r, g, b, a);
}

void RNDR_PLOT(float x, float y) { render_plot(RENDERER, x, y); }

void RNDR_RECT_WH(float x, float y, float w, float h) {
    render_rect_wh(RENDERER, x, y, w, h);
}

void RNDR_RECT_XY(float x1, float y1, float x2, float y2) {
    render_rect_xy(RENDERER, x1, y1, x2, y2);
}

void RNDR_FILL() { render_fill(RENDERER); }

void RNDR_CLEAR() { render_clear(RENDERER); }

void RNDR_FLUSH() { render_flush(RENDERER); }
