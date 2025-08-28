#ifndef DRAW_H
#define DRAW_H

#include "declare.h"
#include <SDL3/SDL_pixels.h>

typedef enum drawtype {
    drawtype_plot = 0,
    drawtype_rect_wh,
    drawtype_rect_xy,
    drawtype_line,
    drawtype_fill,
    drawtype_fade,
    drawtype_color,
    drawtype_clear,
    drawtype_flush,
    
    
    
    drawtype_count,
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
