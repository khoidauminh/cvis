#ifndef CVIS_PROGRAM_H
#define CVIS_PROGRAM_H

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

Program *PG_GET();
Config *PG_CONFIG();
bool PG_KEYPRESSED(KeyEvent);

#include "audio.h"

constexpr uint REFRESHRATE_MAX = 192;
constexpr uint REFRESHRATE_DEFAULT = 60;
constexpr uint ROTATESIZE_DEFAULT = SAMPLERATE * 50 / 1000;

#endif
