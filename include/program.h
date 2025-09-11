#ifndef CVIS_PROGRAM_H
#define CVIS_PROGRAM_H

#include "audio.h"
#include "common.h"

constexpr uint REFRESHRATE_MAX = 192;
constexpr uint REFRESHRATE_DEFAULT = 60;
constexpr uint ROTATESIZE_DEFAULT = SAMPLERATE * 50 / 1000;

typedef enum keyevent {
    keyevents_left,
    keyevents_right,
    keyevents_up,
    keyevents_down,

    keyevents_z,
    keyevents_x,
    keyevents_c,

    keyevent_null,
} KeyEvent;

typedef struct program Program;

#include "config.h"
#include "render.h"

Program *pg_new(Config);

void pg_eventloop(Program *);

void pg_keymap_set(Program *, KeyEvent, bool);
bool pg_keymap_get(Program *, KeyEvent);
void pg_keymap_print(Program *p);
void pg_keymap_reset(Program *p);

Config *pg_config(Program *p);
Renderer *pg_renderer(Program *p);
void pg_attach_renderer(Program *p, Renderer *r);
void pg_end(Program *p);

#include "visualizer.h"

VisManager *pg_vismanager(Program *p);

const Config *PG_CONFIG();
const VisManager *PG_VISMANAGER();
Program *PG_GET();

#endif
