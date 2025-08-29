#ifndef RENDER_H
#define RENDER_H

#include "common.h"

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_pixels.h>

typedef union api_paremeter {
    float rect[4];
    float plot[2];
    float line[4];
    SDL_Color color;
    uint fade;
    uint resize[2];
} APIParameter;

typedef enum renderertype {
    renderertype_sdl,
    renderertype_raylib,
    renderertype_terminal,
} RendererType;

typedef struct renderer Renderer;

typedef void(DrawFunc)(Renderer*, APIParameter*);

constexpr uint DEFAULE_SCALE = 2;
constexpr uint DEFAULT_WIN_SIZE = 84;

#include "config.h"

Renderer *renderer_new(Config *cfg);
void renderer_end(Renderer *);

RendererType renderer_get_type(Renderer *r);
uint renderer_get_width(Renderer *);
uint renderer_get_height(Renderer *);
Size renderer_get_size(Renderer *r);

void render_set_color(Renderer *renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void render_plot(Renderer *r, float x, float y);
void render_rect(Renderer *r, float x, float y, float w, float h);
void render_fill(Renderer *r);
void render_clear(Renderer *r);
void render_flush(Renderer *r);
void render_autoresize(Renderer*);

// helper functions to set a render target only once.
void RNDR_SET_TARGET(Renderer *r);
Size RNDR_SIZE();
void RNDR_COLOR(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void RNDR_PLOT(float x, float y);
void RNDR_RECT(float x , float y, float w, float h);
void RNDR_FILL();
void RNDR_CLEAR();
void RNDR_FLUSH();
void RNDR_AUTORESIZE();

#endif
