#ifndef CVIS_RENDER_H
#define CVIS_RENDER_H

#include "common.h"

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>

typedef enum renderertype {
    renderertype_sdl,
    renderertype_terminal,
} RendererType;

typedef struct renderer Renderer;
typedef union api_paremeter APIParameter;

typedef void(DrawFunc)(Renderer *, APIParameter *);

void RNDR_COLOR(SDL_Color c);
void RNDR_PLOT(float x, float y);
void RNDR_RECT(float x, float y, float w, float h);
void RNDR_LINE(float x1, float y1, float x2, float y2);
void RNDR_FADE(Uint8 a);
void RNDR_BLEND(SDL_BlendMode blendmode);
void RNDR_FILL();
void RNDR_CLEAR();

Uint2D RNDR_SIZE();

#endif
