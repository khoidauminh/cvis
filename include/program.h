#ifndef CVIS_INTERNAL_PROGRAM_H
#define CVIS_INTERNAL_PROGRAM_H

#include "public/program.h" // IWYU pragma: shared

#include "public/config.h"

Program *pg_new(Config);

void pg_eventloop(Program *);
void pg_keymap_set(Program *, KeyEvent, bool);
bool pg_keymap_get(Program *, KeyEvent);
void pg_keymap_print(Program *p);
void pg_keymap_reset(Program *p);

void pg_end(Program *p);

void PG_SET_TARGET(Program *p);

#include "visualizer.h"

Config *pg_config(Program *p);

VisManager *pg_vismanager(Program *p);

#include "public/render.h"

SDLRenderer *PG_RENDERER();

#endif
