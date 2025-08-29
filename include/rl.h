#ifndef RL_H
#define RL_H

#include "render.h"
#include "program.h"

void raylib_init(Renderer*);
void raylib_end(Renderer*);
void pg_eventloop_raylib(Program *p);

#endif
