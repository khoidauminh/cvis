#ifndef CVIS_RENDER_H
#define CVIS_RENDER_H

#include "common.h"

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>

typedef struct sdl_renderer SDLRenderer;

#include "program.h"

SDLRenderer *sdl_renderer_new(Program *prog);
void pg_eventloop_sdl(Program *p);
void sdl_renderer_end(SDLRenderer *sdlr);

void RNDR_COLOR(SDL_Color c);
void RNDR_PLOT(float x, float y);
void RNDR_RECT(float x, float y, float w, float h);
void RNDR_LINE(float x1, float y1, float x2, float y2);
void RNDR_FADE(Uint8 a);
void RNDR_BLEND(SDL_BlendMode blendmode);
void RNDR_FILL();
void RNDR_CLEAR();
void RNDR_TEXT(float x, float y, const char *str, TextAlignment align,
               TextAnchor anchor);
Uint2D RNDR_SIZE();

#endif
