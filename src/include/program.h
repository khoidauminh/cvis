#ifndef PROGRAM_H
#define PROGRAM_H

#include "declare.h"
#include "render.h"

constexpr uint REFRESHRATE_MAX = 192 * 1000;
constexpr uint REFRESHRATE_DEFAULT = 60 * 1000;
constexpr uint ROTATESIZE_DEFAULT = 256;

typedef struct program Program;

Renderer *pg_renderer(Program *p);
Program *pg_new(RendererType, uint rr);
void pg_attach_renderer(Program *p, Renderer *r);
void pg_end(Program *p);

#endif
