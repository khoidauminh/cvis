#ifndef CVIS_CONFIG_H
#define CVIS_CONFIG_H

#include "common.h"

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>

typedef enum refreshmode {
    CVIS_REFRESHMODE_SYNC,
    CVIS_REFRESHMODE_SET,
} eRefreshMode;

typedef struct config {
    eRefreshMode refreshmode;

    tUint refreshrate;
    tUint width;
    tUint height;
    tUint scale;

    bool resizable;

    const char *visname;

    sColor background;
} sConfig;

#endif
