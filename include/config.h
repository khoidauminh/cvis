#ifndef CVIS_CONFIG_H
#define CVIS_CONFIG_H

#include "common.h"

#include <SDL3/SDL_pixels.h>

typedef enum refreshmode {
    refreshmode_sync,
    refreshmode_set,
} RefreshMode;

typedef enum displaymode {
    displaymode_graphical,
    displaymode_terminal,
} DisplayMode;

typedef struct config {
    DisplayMode displaymode;
    RefreshMode refreshmode;

    uint refreshrate;
    uint width;
    uint height;
    uint scale;

    const char *visname;

    SDL_Color background;
} Config;

#include "render.h"

Config config_default();
Config config_parse_args(const int argc, const char **argv);
void config_print(const Config *);

RendererType displaymode_get_renderer(DisplayMode d);

#endif
