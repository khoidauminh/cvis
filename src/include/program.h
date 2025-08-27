#ifndef PROGRAM_H
#define PROGRAM_H

#include "declare.h"
#include "render.h"

constexpr uint REFRESHRATE_MAX = 192 * 1000;
constexpr uint REFRESHRATE_DEFAULT = 60 * 1000;
constexpr uint ROTATESIZE_DEFAULT = 256;

typedef struct program {
    Renderer *renderer;
    uint refreshrate;
} Program;

Program *program_new(RendererType, uint rr);

#endif
