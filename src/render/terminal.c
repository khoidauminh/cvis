#include "terminal.h"
#include "declare.h"
#include "draw.h"
#include "render.h"

#include <assert.h>
#include <ncurses.h>

#include <stdlib.h>

#include <unistdio.h>

void terminal_draw_plot(Renderer *, DrawParameter *);
void terminal_draw_rect_wh(Renderer *, DrawParameter *);
void terminal_draw_rect_xy(Renderer *, DrawParameter *);
void terminal_clear(Renderer *, DrawParameter *);
void terminal_flush(Renderer *, DrawParameter *);
void terminal_set_color(Renderer *, DrawParameter *);

static DrawFunc *TERMINAL_DRAW_FUNC_MAP[] = {
    [dt_plot] = &terminal_draw_plot,
    [dt_rect_wh] = &terminal_draw_rect_wh,
    [dt_rect_xy] = &terminal_draw_rect_xy,
    [dt_flush] = &terminal_flush,
    [dt_color] = &terminal_set_color,
    [dt_fill] = &terminal_clear,
};

typedef struct terminal_renderer {
    WINDOW *win;
    char ch;
} TRenderer;

void terminal_renderer_init(Renderer *r) {

    TRenderer *tr = malloc(sizeof(TRenderer));
    assert(tr);

    tr->win = initscr();
    tr->ch = '*';

    r->renderer = tr;

    raw();
    noecho();

    r->api = TERMINAL_DRAW_FUNC_MAP;
}

void terminal_renderer_end(Renderer *r) {
    endwin();
    free(r->renderer);
    r->renderer = NULL;
}

void terminal_set_color(Renderer *r, DrawParameter *c) {
    TRenderer *tr = r->renderer;
    const char *ch_map = " -+#@";
    const uint ch_map_size = strlen(ch_map);

    uint gray =
        (c->color.r + c->color.g + c->color.b * 2) * c->color.a / 256 / 4;
    uint index = gray * ch_map_size / 256;

    tr->ch = ch_map[index];
}

void terminal_draw_plot(Renderer *r, DrawParameter *param) {
    TRenderer *tr = r->renderer;
    int x = (int)param->plot[0] + 1;
    int y = (int)param->plot[1] + 1;

    mvwaddch(tr->win, y, x, tr->ch);
}

void terminal_draw_rect_wh(Renderer *r, DrawParameter *param) {
    TRenderer *tr = r->renderer;

    int startx = (int)param->rect_wh[0];
    int starty = (int)param->rect_wh[1];
    int w = (int)param->rect_wh[2];
    int h = (int)param->rect_wh[3];

    if (w <= 0 || h <= 0) {
        return;
    }

    char *line = malloc(sizeof(char) * (uint)w);
    memset(line, tr->ch, sizeof(char) * (uint)w);

    for (int row = 0; row < h; row++) {
        mvwprintw(tr->win, starty + row, startx, "%s", line);
    }

    free(line);
}

void terminal_draw_rect_xy(Renderer *r, DrawParameter *param) {
    TRenderer *tr = r->renderer;
    int startx = (int)param->rect_xy[0];
    int starty = (int)param->rect_xy[1];
    int endx = (int)param->rect_xy[2];
    int endy = (int)param->rect_xy[3];

    int w = endx - startx + 1;
    int h = endy - starty + 1;
    if (w <= 0 || h <= 0) {
        return;
    }

    char *line = malloc(sizeof(char) * (uint)w);
    memset(line, tr->ch, sizeof(char) * (uint)w);

    for (int row = 0; row < h; row++) {
        mvwprintw(tr->win, starty + row, startx, "%s", line);
    }

    free(line);
}

void terminal_clear(Renderer *, DrawParameter *) { clear(); }

void terminal_flush(Renderer *, DrawParameter *) { refresh(); }
