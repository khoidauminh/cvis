#ifndef RENDERER_PRIVATE_H
#define RENDERER_PRIVATE_H

#include "render.h"
#include "declare.h"
#include <SDL3/SDL_pixels.h>

struct renderer {
    enum renderer_type type;
    void *renderer;
    uint width;
    uint height;
    uint scale;
    SDL_Color background;
    DrawFunc **api;
};

#endif
