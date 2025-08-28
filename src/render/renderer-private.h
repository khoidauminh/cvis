#ifndef RENDERER_PRIVATE_H
#define RENDERER_PRIVATE_H

#include "render.h"

struct renderer {
    RendererType type;
    void *renderer;
    Config *cfg;
    DrawFunc **api;
};

#endif
