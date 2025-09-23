#ifndef CVIS_PROGRAM_H
#define CVIS_PROGRAM_H

typedef enum keyevent : unsigned {
    KEYEVENT_LEFT = 0,
    KEYEVENT_RIGHT,
    KEYEVENT_UP,
    KEYEVENT_DOWN,

    KEYEVENT_Z,
    KEYEVENT_X,
    KEYEVENT_C,

    KEYEVENT_NULL,
} eKeyEvent;

typedef struct program sProgram;

#include "config.h"

sProgram *PG_GET();
sConfig *PG_CONFIG();
bool PG_KEYPRESSED(eKeyEvent);

#include "audio.h"

constexpr tUint REFRESHRATE_MAX = 192;
constexpr tUint REFRESHRATE_DEFAULT = 60;
constexpr tUint ROTATESIZE_DEFAULT = SAMPLERATE * 50 / 1000;

#endif
