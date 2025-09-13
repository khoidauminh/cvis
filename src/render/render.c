#include "render.h"
#include "common.h"
#include "config.h"
#include "logging.h"

#include "render-private.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>

#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>

#include <SDL3/SDL_render.h>

#include <SDL3/SDL_video.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <assert.h>
#include <stdlib.h>
#include <time.h>

constexpr time_t CURSORHIDE = 1;

struct sdl_renderer {
    SDL_Window *window;
    SDL_Renderer *renderer;
    Config *cfg;
};

void sdlfont_draw_str(SDL_Renderer *render, const char *str, float x, float y,
                      TextAlignment align, TextAnchor anchor);

SDLRenderer *sdl_renderer_new(Program *prog) {
    SDLRenderer *sdlr = malloc(sizeof(SDLRenderer));
    assert(sdlr);

    Config *const cfg = pg_config(prog);

    if (!SDL_Init(SDL_INIT_VIDEO))
        goto ERROR;

    bool result;

    const SDL_WindowFlags flags =
        SDL_WINDOW_ALWAYS_ON_TOP | (cfg->resizable ? SDL_WINDOW_RESIZABLE : 0);

    const uint width = cfg->width;
    const uint height = cfg->height;
    const uint scale = cfg->scale;

    result = SDL_CreateWindowAndRenderer("cvis", (int)(width * scale),
                                         (int)(height * scale), flags,
                                         &sdlr->window, &sdlr->renderer);

    // kinda ugly tbh but it does the work.
    if (!result)
        goto ERROR;

    result = SDL_SetWindowMinimumSize(sdlr->window, MIN_PHYSICAL_SIZE,
                                      MIN_PHYSICAL_SIZE);
    if (!result) {
        warn("Failed to set min window size.");
    }

    result = SDL_SetRenderLogicalPresentation(sdlr->renderer, (int)width,
                                              (int)height, SCALE_MODE);

    if (!result)
        goto ERROR;

    if (pg_config(prog)->refreshmode == CVIS_REFRESHMODE_SYNC) {
        result = SDL_SetRenderVSync(sdlr->renderer, 1);

        const SDL_DisplayMode *mode =
            SDL_GetCurrentDisplayMode(SDL_GetDisplayForWindow(sdlr->window));

        if (mode && mode->refresh_rate >= 0.0f) {
            const uint rate = (uint)mode->refresh_rate;
            cfg->refreshrate = rate;
            info("Detected refresh rate: %u\n", rate);
        }
    } else {
        result = SDL_SetRenderVSync(sdlr->renderer, 0);
    }

    if (!result)
        goto ERROR;

    SDL_SetRenderDrawBlendMode(sdlr->renderer, SDL_BLENDMODE_NONE);

    sdlr->cfg = cfg;

    return sdlr;

ERROR:
    die("%s\n", SDL_GetError());
}

void sdl_renderer_end(SDLRenderer *sdlr) {
    SDL_DestroyWindow(sdlr->window);
    SDL_DestroyRenderer(sdlr->renderer);
}

#include "program.h"

static void update_keyevent(Program *p, SDL_KeyboardEvent *e, bool set) {
    switch (e->key) {
    case SDLK_LEFT:
        pg_keymap_set(p, KLEFT, set);
        break;
    case SDLK_RIGHT:
        pg_keymap_set(p, KRIGHT, set);
        break;
    case SDLK_UP:
        pg_keymap_set(p, KUP, set);
        break;
    case SDLK_DOWN:
        pg_keymap_set(p, KDOWN, set);
        break;
    case SDLK_Z:
        pg_keymap_set(p, KZ, set);
        break;
    case SDLK_X:
        pg_keymap_set(p, KX, set);
        break;
    case SDLK_C:
        pg_keymap_set(p, KC, set);
        break;
    default:
        break;
    }
}

void pg_eventloop_sdl(Program *p) {
    bool running = true;

    PG_SET_TARGET(p);

    time_t cursorhide = 0;

    while (running) {
        time_t now = time(0);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;

            case SDL_EVENT_MOUSE_MOTION:
                SDL_ShowCursor();
                cursorhide = now + CURSORHIDE;

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

        if (cursorhide <= now) {
            SDL_HideCursor();
        }

        vm_perform(p);

        RNDR_FLUSH();

        if (pg_config(p)->refreshmode == CVIS_REFRESHMODE_SET)
            SDL_DelayPrecise(1'000'000'000 / pg_config(p)->refreshrate);
    }
}

Uint2D RNDR_SIZE() {
    return (Uint2D){.x = PG_CONFIG()->width, .y = PG_CONFIG()->height};
}

void RNDR_COLOR(Color c) {
    SDL_SetRenderDrawColor(PG_RENDERER()->renderer, c.r, c.g, c.b, c.a);
}

void RNDR_PLOT(float x, float y) {
    SDL_RenderPoint(PG_RENDERER()->renderer, x, y);
}

void RNDR_RECT(float x, float y, float w, float h) {
    SDL_RenderFillRect(PG_RENDERER()->renderer, &(SDL_FRect){x, y, w, h});
}

void RNDR_LINE(float x1, float y1, float x2, float y2) {
    SDL_RenderLine(PG_RENDERER()->renderer, x1, y1, x2, y2);
}

void RNDR_FADE(Uint8 a) {
    SDLRenderer *sdlr = PG_RENDERER();
    Color newc = sdlr->cfg->background;
    newc.a = a;
    RNDR_COLOR(newc);
    SDL_BlendMode b = SDL_BLENDMODE_NONE;
    SDL_GetRenderDrawBlendMode(sdlr->renderer, &b);
    SDL_SetRenderDrawBlendMode(sdlr->renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(sdlr->renderer, nullptr);
    SDL_SetRenderDrawBlendMode(sdlr->renderer, b);
}

void RNDR_FILL() { SDL_RenderClear(PG_RENDERER()->renderer); }

void RNDR_CLEAR() {
    SDLRenderer *sdlr = PG_RENDERER();
    Color c = sdlr->cfg->background;
    SDL_SetRenderDrawColor(sdlr->renderer, c.r, c.g, c.b, c.a);
    SDL_RenderClear(sdlr->renderer);
}

void RNDR_BLEND(SDL_BlendMode blendmode) {
    SDL_SetRenderDrawBlendMode(PG_RENDERER()->renderer, blendmode);
}

void RNDR_FLUSH() {
    SDL_RenderPresent(PG_RENDERER()->renderer);
    RNDR_BLEND(SDL_BLENDMODE_NONE);
}

void RNDR_TEXT(float x, float y, const char *str, TextAlignment align,
               TextAnchor anchor) {
    sdlfont_draw_str(PG_RENDERER()->renderer, str, x, y, align, anchor);
}

void RNDR_AUTORESIZE() {
    int w, h;
    SDLRenderer *sdlr = PG_RENDERER();
    SDL_GetWindowSize(sdlr->window, &w, &h);
    sdlr->cfg->width = uint_min(MAX_LOGICAL_SIZE, (uint)w / sdlr->cfg->scale);
    sdlr->cfg->height = uint_min(MAX_LOGICAL_SIZE, (uint)h / sdlr->cfg->scale);
    SDL_SetRenderLogicalPresentation(sdlr->renderer, (int)sdlr->cfg->width,
                                     (int)sdlr->cfg->height, SCALE_MODE);
}
