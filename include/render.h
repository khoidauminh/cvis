#ifndef CVIS_INTERNAL_RENDER_H
#define CVIS_INTERNAL_RENDER_H

#include "public/program.h"
#include "public/render.h"

SDLRenderer *sdl_renderer_new(Program *prog);
void pg_eventloop_sdl(Program *p);
void sdl_renderer_end(SDLRenderer *sdlr);

void RNDR_SET_TARGET(SDLRenderer *r);

void RNDR_FLUSH();
void RNDR_AUTORESIZE();

#endif
