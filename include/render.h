#ifndef CVIS_INTERNAL_RENDER_H
#define CVIS_INTERNAL_RENDER_H

#include "public/program.h"
#include "public/render.h"

sRenderer *sdl_renderer_new(sProgram *prog);
void pg_eventloop_sdl(sProgram *p);
void sdl_renderer_end(sRenderer *sdlr);

void RNDR_SET_TARGET(sRenderer *r);

void RNDR_FLUSH();
void RNDR_AUTORESIZE();

#endif
