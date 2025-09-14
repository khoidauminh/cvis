#ifndef CVIS_RENDER_H
#define CVIS_RENDER_H

#include "common.h"

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>

typedef struct sdl_renderer SDLRenderer;

void RNDR_COLOR(Color c);
void RNDR_PLOT(float x, float y);
void RNDR_RECT(float x, float y, float w, float h);
void RNDR_LINE(float x1, float y1, float x2, float y2);
void RNDR_FADE(ubyte a);
void RNDR_BLEND(BlendMode blendmode);
void RNDR_FILL();
void RNDR_CLEAR();
void RNDR_TEXT(float x, float y, const char *str, TextAlignment align,
               TextAnchor anchor);
Uint2D RNDR_SIZE();

#endif
