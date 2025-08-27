#include "window.h"
#include "draw.h"
#include "render.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

#include <assert.h>
#include <stdlib.h>

typedef struct window_renderer {
    SDL_Window *window;
    SDL_Renderer *renderer;
} WRenederer;

void window_set_color(Renderer *, DrawParameter *);
void window_draw_plot(Renderer *, DrawParameter *);
void window_draw_rect_wh(Renderer *, DrawParameter *);
void window_draw_rect_xy(Renderer *, DrawParameter *);
void window_clear(Renderer *, DrawParameter *);
void window_present(Renderer *, DrawParameter *);

static DrawFunc *SDL_DRAW_FUNC_MAP[] = {
    [dt_plot] = &window_draw_plot,       [dt_rect_wh] = &window_draw_rect_wh,
    [dt_rect_xy] = &window_draw_rect_xy, [dt_fill] = &window_clear,
    [dt_color] = &window_set_color,      [dt_flush] = &window_present,
};

void window_renderer_init(Renderer *r) {
    WRenederer *wr = malloc(sizeof(WRenederer));
    assert(wr);

    assert(SDL_Init(SDL_INIT_VIDEO));

    SDL_WindowFlags flags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALWAYS_ON_TOP;

    assert(SDL_CreateWindowAndRenderer("cvis", (int)r->width * 2,
                                       (int)r->height * 2, flags, &wr->window,
                                       &wr->renderer));

    SDL_SetRenderLogicalPresentation(wr->renderer, (int)r->width,
                                     (int)r->height,
                                     SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

    assert(SDL_SetRenderVSync(wr->renderer, 1));

    r->renderer = wr;
    r->api = SDL_DRAW_FUNC_MAP;
}

void window_renderer_end(Renderer *r) {
    WRenederer *wr = r->renderer;
    SDL_DestroyWindow(wr->window);
    SDL_DestroyRenderer(wr->renderer);
    r->renderer = NULL;
    free(wr);
}

void window_set_color(Renderer *r, DrawParameter *c) {
    WRenederer *wr = r->renderer;
    SDL_SetRenderDrawColor(wr->renderer, c->color.r, c->color.g, c->color.b,
                           c->color.a);
}

void window_draw_plot(Renderer *r, DrawParameter *param) {
    WRenederer *wr = r->renderer;
    SDL_RenderPoint(wr->renderer, param->plot[0], param->plot[1]);
}

void window_draw_rect_wh(Renderer *r, DrawParameter *param) {
    WRenederer *wr = r->renderer;

    SDL_FRect rect = {param->rect_wh[0], param->rect_wh[1], param->rect_wh[2],
                      param->rect_wh[3]};

    SDL_RenderFillRect(wr->renderer, &rect);
}

void window_draw_rect_xy(Renderer *r, DrawParameter *param) {
    WRenederer *wr = r->renderer;

    SDL_FRect rect = {param->rect_wh[0], param->rect_wh[1],
                      param->rect_wh[2] - param->rect_wh[0],
                      param->rect_wh[3] - param->rect_wh[1]};

    SDL_RenderFillRect(wr->renderer, &rect);
}

void window_clear(Renderer *r, DrawParameter *) {
    WRenederer *wr = r->renderer;
    SDL_RenderClear(wr->renderer);
}

void window_present(Renderer *r, DrawParameter *) {
    WRenederer *wr = r->renderer;
    SDL_RenderPresent(wr->renderer);
}
