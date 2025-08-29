#ifndef SDL_H
#define SDL_H

#include "render.h"
#include "program.h"

void sdl_renderer_init(Renderer *r);
void sdl_renderer_end(Renderer *r);
void pg_eventloop_sdl(Program *p);

#endif
