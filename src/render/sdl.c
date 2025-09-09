#include "config.h"
#include "logging.h"
#include "render.h"
#include "renderer-private.h" // IWYU pragma: keep.

#include <SDL3/SDL.h>
#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>

#include <assert.h>
#include <ncurses.h>
#include <stdlib.h>

typedef struct sdl_renderer {
    SDL_Window *window;
    SDL_Renderer *renderer;
} SDLRenederer;

static void sdl_set_color(Renderer *rndr, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDLRenederer *sdlr = rndr->renderer;
    SDL_SetRenderDrawColor(sdlr->renderer, r, g, b, a);
}

static void sdl_draw_plot(Renderer *r, float x, float y) {
    SDLRenederer *sdlr = r->renderer;
    SDL_RenderPoint(sdlr->renderer, x, y);
}

static void sdl_draw_line(Renderer *r, float x1, float y1, float x2, float y2) {
    SDLRenederer *sdlr = r->renderer;
    SDL_RenderLine(sdlr->renderer, x1, y1, x2, y2);
}

static void sdl_draw_rect(Renderer *r, float x, float y, float w, float h) {
    SDLRenederer *sdlr = r->renderer;
    SDL_RenderFillRect(sdlr->renderer, &(SDL_FRect){x, y, w, h});
}

static void sdl_fill(Renderer *r) {
    SDLRenederer *sdlr = r->renderer;
    SDL_RenderClear(sdlr->renderer);
}

static void sdl_fade(Renderer *r, Uint8 a) {
    SDLRenederer *sdlr = r->renderer;
    SDL_FRect rect = {.x = 0.0f,
                      .y = 0.0f,
                      .w = (float)r->cfg->width,
                      .h = (float)r->cfg->height};

    SDL_Color newc = r->cfg->background;
    newc.a = a;
    sdl_set_color(r, newc.r, newc.g, newc.b, newc.a);
    SDL_BlendMode b = SDL_BLENDMODE_NONE;
    SDL_GetRenderDrawBlendMode(sdlr->renderer, &b);
    SDL_SetRenderDrawBlendMode(sdlr->renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(sdlr->renderer, &rect);
    SDL_SetRenderDrawBlendMode(sdlr->renderer, b);
}

static void sdl_clear(Renderer *r) {
    SDLRenederer *sdlr = r->renderer;
    SDL_Color c = r->cfg->background;
    SDL_SetRenderDrawColor(sdlr->renderer, c.r, c.g, c.b, c.a);
    SDL_RenderClear(sdlr->renderer);
}

static void sdl_present(Renderer *r) {
    SDLRenederer *sdlr = r->renderer;
    SDL_RenderPresent(sdlr->renderer);
}

static void sdl_set_blendmode(Renderer *r, SDL_BlendMode blendmode) {
    SDLRenederer *sdlr = r->renderer;
    SDL_SetRenderDrawBlendMode(sdlr->renderer, blendmode);
}

static void sdl_autoresize(Renderer *r) {
    SDLRenederer *sdlr = r->renderer;
    int w, h;
    SDL_GetWindowSize(sdlr->window, &w, &h);
    r->cfg->width = uint_min(MAX_LOGICAL_SIZE, (uint)w / r->cfg->scale);
    r->cfg->height = uint_min(MAX_LOGICAL_SIZE, (uint)h / r->cfg->scale);
    SDL_SetRenderLogicalPresentation(sdlr->renderer, (int)r->cfg->width,
                                     (int)r->cfg->height, SCALE_MODE);
}

static const RenderVTable SDL_VTABLE = {
    .blend = sdl_set_blendmode,
    .clear = sdl_clear,
    .color = sdl_set_color,
    .fill = sdl_fill,
    .flush = sdl_present,
    .plot = sdl_draw_plot,
    .line = sdl_draw_line,
    .fade = sdl_fade,
    .rect = sdl_draw_rect,
    .resize = sdl_autoresize,
};

void sdl_renderer_init(Renderer *r) {
    SDLRenederer *sdlr = malloc(sizeof(SDLRenederer));
    assert(sdlr);

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        die("Failed to initialize SDL Video.");
    }

    bool result;

    unsigned long flags = SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_OPENGL;

    if (r->cfg->resizable) {
        flags |= SDL_WINDOW_RESIZABLE;
    }

    result = SDL_CreateWindowAndRenderer("cvis",
                                         (int)(r->cfg->width * r->cfg->scale),
                                         (int)(r->cfg->height * r->cfg->scale),
                                         flags, &sdlr->window, &sdlr->renderer);

    if (!result) {
        die("Failed to create SDL window and renderer.");
    }

    result = SDL_SetWindowMinimumSize(sdlr->window, MIN_PHYSICAL_SIZE,
                                      MIN_PHYSICAL_SIZE);
    if (!result) {
        warn("Failed to set min window size.");
    }

    result = SDL_SetRenderLogicalPresentation(
        sdlr->renderer, (int)r->cfg->width, (int)r->cfg->height, SCALE_MODE);

    if (!result) {
        die("Failed to set renderer resolution.");
    }

    if (r->cfg->refreshmode == CVIS_REFRESHMODE_SYNC)
        result = SDL_SetRenderVSync(sdlr->renderer, 1);
    else
        result = SDL_SetRenderVSync(sdlr->renderer, 0);

    if (!result) {
        die("Failed to set configure Vsync.");
    }

    SDL_SetRenderDrawBlendMode(sdlr->renderer, SDL_BLENDMODE_NONE);

    r->renderer = sdlr;

    r->vtable = &SDL_VTABLE;
}

void sdl_renderer_end(Renderer *r) {
    SDLRenederer *sdlr = r->renderer;
    SDL_DestroyWindow(sdlr->window);
    SDL_DestroyRenderer(sdlr->renderer);
    r->renderer = nullptr;
    free(sdlr);
}

#include "program.h"

static void update_keyevent(Program *p, SDL_KeyboardEvent *e, bool set) {
    switch (e->key) {
    case SDLK_LEFT:
        pg_keymap_set(p, keyevents_left, set);
        break;
    case SDLK_RIGHT:
        pg_keymap_set(p, keyevents_right, set);
        break;
    case SDLK_UP:
        pg_keymap_set(p, keyevents_up, set);
        break;
    case SDLK_DOWN:
        pg_keymap_set(p, keyevents_down, set);
        break;
    case SDLK_Z:
        pg_keymap_set(p, keyevents_z, set);
        break;
    case SDLK_X:
        pg_keymap_set(p, keyevents_x, set);
        break;
    case SDLK_C:
        pg_keymap_set(p, keyevents_c, set);
        break;
    default:
        break;
    }
}

void pg_eventloop_sdl(Program *p) {
    assert(renderer_get_type(pg_renderer(p)) == renderertype_sdl);

    bool running = true;

    RNDR_SET_TARGET(pg_renderer(p));

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            case SDL_EVENT_KEY_DOWN:

                switch (event.key.key) {
                case SDLK_SPACE:
                    vm_next(pg_vismanager(p));
                    RNDR_CLEAR();
                    break;
                case SDLK_Q:
                    running = false;
                    break;
                default:
                    update_keyevent(p, &event.key, true);
                }

                break;

            case SDL_EVENT_KEY_UP:
                update_keyevent(p, &event.key, false);
                break;

            case SDL_EVENT_WINDOW_RESIZED:
                RNDR_AUTORESIZE();
                break;
            default: {
            }
            }
        }

        vm_perform(p);

        RNDR_FLUSH();

        if (pg_config(p)->refreshmode == CVIS_REFRESHMODE_SET)
            SDL_DelayPrecise(1'000'000'000 / pg_config(p)->refreshrate);
    }
}
