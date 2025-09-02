#include "logging.h"
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

static void sdl_set_color(Renderer *r, APIParameter *c) {
    SDLRenederer *sdlr = r->renderer;
    SDL_SetRenderDrawColor(sdlr->renderer, c->color.r, c->color.g, c->color.b,
                           c->color.a);
}

static void sdl_draw_plot(Renderer *r, APIParameter *param) {
    SDLRenederer *sdlr = r->renderer;
    SDL_RenderPoint(sdlr->renderer, param->plot[0], param->plot[1]);
}

static void sdl_draw_rect(Renderer *r, APIParameter *param) {
    SDLRenederer *sdlr = r->renderer;

    SDL_FRect rect = {param->rect[0], param->rect[1], param->rect[2],
                      param->rect[3]};

    SDL_RenderFillRect(sdlr->renderer, &rect);
}

static void sdl_fill(Renderer *r, APIParameter *) {
    SDLRenederer *sdlr = r->renderer;
    SDL_RenderClear(sdlr->renderer);
}

static void sdl_clear(Renderer *r, APIParameter *) {
    SDLRenederer *sdlr = r->renderer;
    SDL_Color c = r->cfg->background;
    SDL_SetRenderDrawColor(sdlr->renderer, c.r, c.g, c.b, c.a);
    SDL_RenderClear(sdlr->renderer);
}

static void sdl_present(Renderer *r, APIParameter *) {
    SDLRenederer *sdlr = r->renderer;
    SDL_RenderPresent(sdlr->renderer);
}

static void sdl_set_blendmode(Renderer *r, APIParameter *param) {
    SDLRenederer *sdlr = r->renderer;
    SDL_SetRenderDrawBlendMode(sdlr->renderer, param->blendmode);
}

static void sdl_autoresize(Renderer *, APIParameter *) {}

static DrawFunc *SDL_DRAW_FUNC_MAP[] = {
    [renderapi_plot] = sdl_draw_plot,      [renderapi_rect] = sdl_draw_rect,
    [renderapi_fill] = sdl_fill,           [renderapi_clear] = sdl_clear,
    [renderapi_color] = sdl_set_color,     [renderapi_flush] = sdl_present,
    [renderapi_blend] = sdl_set_blendmode, [renderapi_resize] = sdl_autoresize,
};

void sdl_renderer_init(Renderer *r) {
    SDLRenederer *sdlr = malloc(sizeof(SDLRenederer));
    assert(sdlr);

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        die("Failed to initialize SDL Video.");
    }

    bool result;

    result = SDL_CreateWindowAndRenderer(
        "cvis", (int)(r->cfg->width * r->cfg->scale),
        (int)(r->cfg->height * r->cfg->scale), SDL_WINDOW_ALWAYS_ON_TOP,
        &sdlr->window, &sdlr->renderer);

    if (!result) {
        die("Failed to create SDL window and renderer.");
    }

    result = SDL_SetRenderLogicalPresentation(
        sdlr->renderer, (int)r->cfg->width, (int)r->cfg->height,
        SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

    if (!result) {
        die("Failed to set renderer resolution.");
    }

    if (r->cfg->refreshmode == refreshmode_sync)
        result = SDL_SetRenderVSync(sdlr->renderer, 1);
    else
        result = SDL_SetRenderVSync(sdlr->renderer, 0);

    if (!result) {
        die("Failed to set configure Vsync.");
    }

    SDL_SetRenderDrawBlendMode(sdlr->renderer, SDL_BLENDMODE_NONE);

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

        RNDR_FLUSH();

        if (pg_config(p)->refreshmode == refreshmode_set)
            SDL_Delay(1000 / pg_config(p)->refreshrate);
    }
}
