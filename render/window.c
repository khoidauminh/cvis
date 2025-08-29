#include "window.h"

#include "render.h"
#include "renderer-private.h" // IWYU pragma: keep.

#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include <assert.h>
#include <stdlib.h>

typedef struct window_renderer {
    SDL_Window *window;
    SDL_Renderer *renderer;
} WRenederer;

void window_set_color(Renderer *r, APIParameter *c) {
    WRenederer *wr = r->renderer;
    SDL_SetRenderDrawColor(wr->renderer, c->color.r, c->color.g, c->color.b,
                           c->color.a);
}

void window_draw_plot(Renderer *r, APIParameter *param) {
    WRenederer *wr = r->renderer;
    SDL_RenderPoint(wr->renderer, param->plot[0], param->plot[1]);
}

void window_draw_rect_wh(Renderer *r, APIParameter *param) {
    WRenederer *wr = r->renderer;

    SDL_FRect rect = {param->rect_wh[0], param->rect_wh[1], param->rect_wh[2],
                      param->rect_wh[3]};

    SDL_RenderFillRect(wr->renderer, &rect);
}

void window_draw_rect_xy(Renderer *r, APIParameter *param) {
    WRenederer *wr = r->renderer;

    SDL_FRect rect = {param->rect_wh[0], param->rect_wh[1],
                      param->rect_wh[2] - param->rect_wh[0],
                      param->rect_wh[3] - param->rect_wh[1]};

    SDL_RenderFillRect(wr->renderer, &rect);
}

void window_fill(Renderer *r, APIParameter *) {
    WRenederer *wr = r->renderer;
    SDL_RenderClear(wr->renderer);
}

void window_clear(Renderer *r, APIParameter *) {
    WRenederer *wr = r->renderer;
    SDL_Color c = r->cfg->background;
    SDL_SetRenderDrawColor(wr->renderer, c.r, c.g, c.b, c.a);
    SDL_RenderClear(wr->renderer);
}

void window_present(Renderer *r, APIParameter *) {
    WRenederer *wr = r->renderer;
    SDL_RenderPresent(wr->renderer);
}

void window_autoresize(Renderer *, APIParameter *) {}

static DrawFunc *SDL_DRAW_FUNC_MAP[] = {
    [renderapi_plot] = &window_draw_plot,
    [renderapi_rect_wh] = &window_draw_rect_wh,
    [renderapi_rect_xy] = &window_draw_rect_xy,
    [renderapi_fill] = &window_fill,
    [renderapi_clear] = &window_clear,
    [renderapi_color] = &window_set_color,
    [renderapi_flush] = &window_present,
    [renderapi_resize] = &window_autoresize,
};

void window_renderer_init(Renderer *r) {
    WRenederer *wr = malloc(sizeof(WRenederer));
    assert(wr);

    assert(SDL_Init(SDL_INIT_VIDEO));

    SDL_WindowFlags flags = SDL_WINDOW_VULKAN | SDL_WINDOW_ALWAYS_ON_TOP;

    assert(SDL_CreateWindowAndRenderer("cvis", r->cfg->width * DEFAULE_SCALE,
                                       r->cfg->height * DEFAULE_SCALE, flags,
                                       &wr->window, &wr->renderer));

    SDL_SetRenderLogicalPresentation(wr->renderer, (int)r->cfg->width,
                                     (int)r->cfg->height,
                                     SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

    if (r->cfg->refreshmode == refreshmode_sync)
        assert(SDL_SetRenderVSync(wr->renderer, 1));
    else
        assert(SDL_SetRenderVSync(wr->renderer, 0));

    r->renderer = wr;
    r->api = SDL_DRAW_FUNC_MAP;
}

void window_renderer_end(Renderer *r) {
    WRenederer *wr = r->renderer;
    SDL_DestroyWindow(wr->window);
    SDL_DestroyRenderer(wr->renderer);
    r->renderer = nullptr;
    free(wr);
}
