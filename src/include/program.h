#ifndef PROGRAM_H
#define PROGRAM_H

#include "audio.h"
#include "declare.h"

constexpr uint REFRESHRATE_MAX = 192;
constexpr uint REFRESHRATE_DEFAULT = 60;
constexpr uint ROTATESIZE_DEFAULT = SAMPLERATE * 50 / 1000;

typedef struct program Program;

#include "config.h"
#include "render.h"

Program *pg_new(Config);

void pg_eventloop_win(Program*);

Renderer *pg_renderer(Program *p);
void pg_attach_renderer(Program *p, Renderer *r);
void pg_end(Program *p);

#include "visualizer.h"

VisManager *pg_vismanager(Program *p);

#endif
