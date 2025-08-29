#include "sdl.h"

#include "render.h"
#include "renderer-private.h" // IWYU pragma: keep.

#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include <assert.h>
#include <stdlib.h>

typedef struct sdl_renderer {
    SDL_Window *window;
    SDL_Renderer *renderer;
} SDLRenederer;

void sdl_set_color(Renderer *r, APIParameter *c) {
    SDLRenederer *sdlr = r->renderer;
    SDL_SetRenderDrawColor(sdlr->renderer, c->color.r, c->color.g, c->color.b,
                           c->color.a);
}

void sdl_draw_plot(Renderer *r, APIParameter *param) {
    SDLRenederer *sdlr = r->renderer;
    SDL_RenderPoint(sdlr->renderer, param->plot[0], param->plot[1]);
}

void sdl_draw_rect(Renderer *r, APIParameter *param) {
    SDLRenederer *sdlr = r->renderer;

    SDL_FRect rect = {param->rect[0], param->rect[1], param->rect[2],
                      param->rect[3]};

    SDL_RenderFillRect(sdlr->renderer, &rect);
}

void sdl_fill(Renderer *r, APIParameter *) {
    SDLRenederer *sdlr = r->renderer;
    SDL_RenderClear(sdlr->renderer);
}

void sdl_clear(Renderer *r, APIParameter *) {
    SDLRenederer *sdlr = r->renderer;
    SDL_Color c = r->cfg->background;
    SDL_SetRenderDrawColor(sdlr->renderer, c.r, c.g, c.b, c.a);
    SDL_RenderClear(sdlr->renderer);
}

void sdl_present(Renderer *r, APIParameter *) {
    SDLRenederer *sdlr = r->renderer;
    SDL_RenderPresent(sdlr->renderer);
}

void sdl_autoresize(Renderer *, APIParameter *) {}

static DrawFunc *SDL_DRAW_FUNC_MAP[] = {
    [renderapi_plot] = &sdl_draw_plot,    [renderapi_rect] = &sdl_draw_rect,
    [renderapi_fill] = &sdl_fill,         [renderapi_clear] = &sdl_clear,
    [renderapi_color] = &sdl_set_color,   [renderapi_flush] = &sdl_present,
    [renderapi_resize] = &sdl_autoresize,
};

void sdl_renderer_init(Renderer *r) {
    SDLRenederer *sdlr = malloc(sizeof(SDLRenederer));
    assert(sdlr);

    assert(SDL_Init(SDL_INIT_VIDEO));

    SDL_WindowFlags flags = SDL_WINDOW_VULKAN | SDL_WINDOW_ALWAYS_ON_TOP;

    assert(SDL_CreateWindowAndRenderer("cvis", r->cfg->width * DEFAULE_SCALE,
                                       r->cfg->height * DEFAULE_SCALE, flags,
                                       &sdlr->window, &sdlr->renderer));

    SDL_SetRenderLogicalPresentation(sdlr->renderer, (int)r->cfg->width,
                                     (int)r->cfg->height,
                                     SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

    if (r->cfg->refreshmode == refreshmode_sync)
        assert(SDL_SetRenderVSync(sdlr->renderer, 1));
    else
        assert(SDL_SetRenderVSync(sdlr->renderer, 0));

    r->renderer = sdlr;
    r->api = SDL_DRAW_FUNC_MAP;
}

void sdl_renderer_end(Renderer *r) {
    SDLRenederer *sdlr = r->renderer;
    SDL_DestroyWindow(sdlr->window);
    SDL_DestroyRenderer(sdlr->renderer);
    r->renderer = nullptr;
    free(sdlr);
}

#include "program.h"

void pg_eventloop_sdl(Program *p) {
    assert(renderer_get_type(pg_renderer(p)) == renderertype_sdl);

    bool running = true;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_SPACE) {
                    vm_next(pg_vismanager(p));
                }

                break;
            default: {
            }
            }
        }

        vm_perform(p);

        if (pg_config(p)->refreshmode == refreshmode_set)
            SDL_Delay(1000 / pg_config(p)->refreshrate);
    }
}
