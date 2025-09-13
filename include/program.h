#ifndef CVIS_PROGRAM_H
#define CVIS_PROGRAM_H

#include "audio.h"
#include "common.h"
#include <SDL3/SDL_render.h>

constexpr uint REFRESHRATE_MAX = 192;
constexpr uint REFRESHRATE_DEFAULT = 60;
constexpr uint ROTATESIZE_DEFAULT = SAMPLERATE * 50 / 1000;

typedef enum keyevent : unsigned {
    KLEFT = 0,
    KRIGHT,
    KUP,
    KDOWN,

    KZ,
    KX,
    KC,

    keyevent_null,
} KeyEvent;

typedef struct program Program;

#include "config.h"

Program *pg_new(Config);

void pg_eventloop(Program *);
void pg_keymap_set(Program *, KeyEvent, bool);
bool pg_keymap_get(Program *, KeyEvent);
void pg_keymap_print(Program *p);
void pg_keymap_reset(Program *p);

void pg_end(Program *p);

#include "visualizer.h"

Config *pg_config(Program *p);
Config *PG_CONFIG();

Program *PG_GET();
VisManager *pg_vismanager(Program *p);

#endif
