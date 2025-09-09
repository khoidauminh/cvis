#ifndef CVIS_CONFIG_H
#define CVIS_CONFIG_H

#include "common.h"

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>

typedef enum refreshmode {
    CVIS_REFRESHMODE_SYNC,
    CVIS_REFRESHMODE_SET,
} RefreshMode;

typedef enum displaymode {
    CVIS_DISPLAYMODE_GRAPHICAL,
    CVIS_DISPLAYMODE_TERMINAL,
} DisplayMode;

typedef struct config {
    DisplayMode displaymode;
    RefreshMode refreshmode;

    uint refreshrate;
    uint width;
    uint height;
    uint scale;

    bool resizable;

    const char *visname;

    SDL_Color background;
} Config;

constexpr uint MIN_PHYSICAL_SIZE = 64;
constexpr uint MAX_LOGICAL_SIZE = 256;
constexpr SDL_RendererLogicalPresentation SCALE_MODE =
    SDL_LOGICAL_PRESENTATION_OVERSCAN;

#include "render.h"

Config config_default();
Config config_parse_args(const int argc, const char **argv);
void config_print(const Config *);

RendererType displaymode_get_renderer(DisplayMode d);

#endif
