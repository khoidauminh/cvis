#ifndef CVIS_INTERNAL_PROGRAM_H
#define CVIS_INTERNAL_PROGRAM_H

#include "public/program.h" // IWYU pragma: shared

#include "public/config.h"

sProgram *pg_new(sConfig);

void pg_eventloop(sProgram *);
void pg_keymap_set(sProgram *, eKeyEvent, bool);
bool pg_keymap_get(sProgram *, eKeyEvent);
void pg_keymap_print(sProgram *p);
void pg_keymap_reset(sProgram *p);

void pg_end(sProgram *p);

void PG_SET_TARGET(sProgram *p);

#include "visualizer.h"

sConfig *pg_config(sProgram *p);

sVisManager *pg_vismanager(sProgram *p);

#include "public/render.h"

sRenderer *PG_RENDERER();

#endif
