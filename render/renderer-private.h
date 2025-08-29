#ifndef RENDERER_PRIVATE_H
#define RENDERER_PRIVATE_H

#include "render.h"

typedef enum renderapi {
    renderapi_plot = 0,
    renderapi_rect_wh,
    renderapi_rect_xy,
    renderapi_line,
    renderapi_fill,
    renderapi_fade,
    renderapi_color,
    renderapi_clear,
    renderapi_flush,
    renderapi_resize,
    
    renderapi_count,
} RenderAPI;

struct renderer {
    RendererType type;
    void *renderer;
    Config *cfg;
    DrawFunc **api;
};

#endif
