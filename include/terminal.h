#ifndef TERMINAL_H
#define TERMINAL_H

#include "render.h"
#include "program.h"

void terminal_renderer_init(Renderer *r);
void terminal_renderer_end(Renderer *r);
void pg_eventloop_term(Program *p);

#endif
