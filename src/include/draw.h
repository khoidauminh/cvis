#ifndef DRAW_H
#define DRAW_H

#include "declare.h"
#include <SDL3/SDL_pixels.h>

typedef enum draw_type {
    dt_plot = 0,
    dt_rect_wh,
    dt_rect_xy,
    dt_line,
    dt_fill,
    dt_fade,
    dt_color,
    dt_clear,
    dt_flush,
    dt_count,
} DrawType;

typedef union draw_parameter {
    float rect_wh[4];
    float rect_xy[4];
    float plot[2];
    float line[4];
    SDL_Color color;
    uint fade;
} DrawParameter;

#endif
