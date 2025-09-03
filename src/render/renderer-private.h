#ifndef CVIS_RENDERER_PRIVATE_H
#define CVIS_RENDERER_PRIVATE_H

#include "render.h"

typedef enum renderapi: uint {
    renderapi_null = 0,
    
    renderapi_plot = 0,
    
    renderapi_rect,
    
    // renderapi_line,
    
    renderapi_fill,
    
    // renderapi_fade,
    
    renderapi_color,
    renderapi_clear,
    renderapi_flush,
    
    renderapi_resize,
    renderapi_blend,
    
    renderapi_count,
} RenderAPI;

typedef union api_paremeter {
    float rect[4];
    float plot[2];
    float line[4];
    SDL_Color color;
    uint fade;
    uint resize[2];
    SDL_BlendMode blendmode;
} APIParameter;

struct renderer {
    RendererType type;
    void *renderer;
    
    void (*init)(Renderer*);
    void (*exit)(Renderer*);
    
    Config *cfg;
    DrawFunc **api;
};

#ifdef USE_RAYLIB
void raylib_init(Renderer *);
void raylib_end(Renderer *);
#endif

void sdl_renderer_init(Renderer *r);
void sdl_renderer_end(Renderer *r);

void terminal_renderer_init(Renderer *r);
void terminal_renderer_end(Renderer *r);

#endif
