#ifndef RENDER_H
#define RENDER_H

#include "declare.h"
#include "draw.h"

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_pixels.h>

typedef enum renderer_type {
    rt_sdl,
    rt_console,
} RendererType;

typedef struct renderer Renderer;

typedef void(DrawFunc)(Renderer*, DrawParameter*);

struct renderer {
    enum renderer_type type;
    void *renderer;
    uint width;
    uint height;
    DrawFunc **api;
};


Renderer *renderer_init(RendererType type, uint width, uint height);
void renderer_end(Renderer *);

uint renderer_get_width(Renderer *);
uint renderer_get_height(Renderer *);
Size renderer_get_size(Renderer *r);

void render_set_color(Renderer *renderer, uint r, uint g, uint b, uint a);
void render_plot(Renderer *r, float x, float y);
void render_rect_wh(Renderer *r, float x, float y, float w, float h);
void render_rect_xy(Renderer *r, float x1, float y1, float x2, float y2);
void render_fill(Renderer *r);
void render_flush(Renderer *r);

// helper functions
void RNDR_SET_TARGET(Renderer *r);

Size RNDR_SIZE();

void RNDR_COLOR(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

void RNDR_PLOT(float x, float y);

void RNDR_RECT_WH(float x , float y, float w, float h);

void RNDR_RECT_XY(float x1, float y1, float x2, float y2);

void RNDR_FILL();

void RNDR_FLUSH();

#endif
