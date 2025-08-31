#ifndef RENDER_H
#define RENDER_H

#include "common.h"

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_pixels.h>

typedef enum renderertype {
    renderertype_sdl,
    renderertype_raylib,
    renderertype_terminal,
} RendererType;

typedef struct renderer Renderer;
typedef union api_paremeter APIParameter;

typedef void(DrawFunc)(Renderer*, APIParameter*);

#include "config.h"

Renderer *renderer_new(Config *cfg);
void renderer_end(Renderer *);

RendererType renderer_get_type(Renderer *r);

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
