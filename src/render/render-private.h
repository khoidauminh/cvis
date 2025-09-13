#ifndef CVIS_RENDER_PRIVATE_H
#define CVIS_RENDER_PRIVATE_H

#include "render.h"
#include <SDL3/SDL_stdinc.h>

#include "program.h"

// PRIVATE HEADERS:
void RNDR_SET_TARGET(SDLRenderer *r);

void PG_SET_TARGET(Program *p);
void RNDR_FLUSH();
void RNDR_AUTORESIZE();

SDLRenderer *PG_RENDERER();

#endif
