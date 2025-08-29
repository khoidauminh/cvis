#include "terminal.h"
#include "common.h"
#include "render.h"
#include "renderer-private.h" // IWYU pragma: keep.

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <locale.h>
#include <ncurses.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <unistdio.h>

constexpr int MAX_STR_LEN = 256;

typedef struct terminal_renderer {
    WINDOW *win;
    char ch;
} TRenderer;

void terminal_set_color(Renderer *r, APIParameter *c) {
    TRenderer *tr = r->renderer;
    const char *ch_map = " -+#@";
    const uint ch_map_size = strlen(ch_map);

    uint gray =
        (c->color.r + c->color.g + c->color.b * 2) * c->color.a / 256 / 4;
    uint index = gray * ch_map_size / 256;

    tr->ch = ch_map[index];
}

void terminal_draw_plot(Renderer *r, APIParameter *param) {
    TRenderer *tr = r->renderer;
    int x = (int)param->plot[0] + 1;
    int y = (int)param->plot[1] + 1;

    mvwaddch(tr->win, y, x, tr->ch);
}

void terminal_draw_rect_wh(Renderer *r, APIParameter *param) {
    TRenderer *tr = r->renderer;

    int startx = (int)param->rect_wh[0];
    int starty = (int)param->rect_wh[1];
    int w = (int)param->rect_wh[2];
    int h = (int)param->rect_wh[3];

    if (w <= 0 || h <= 0) {
        return;
    }

    char line[MAX_STR_LEN] = {};
    uint copysize = w < MAX_STR_LEN ? w : MAX_STR_LEN;

    memset(line, tr->ch, sizeof(char) * copysize);

    for (int row = 0; row < h; row++) {
        mvwprintw(tr->win, starty + row, startx, "%s", line);
    }
}

void terminal_draw_rect_xy(Renderer *r, APIParameter *param) {
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

    char line[MAX_STR_LEN] = {};
    uint copysize = w < MAX_STR_LEN ? w : MAX_STR_LEN;

    memset(line, tr->ch, sizeof(char) * copysize);

    for (int row = 0; row < h; row++) {
        mvwprintw(tr->win, starty + row, startx, "%s", line);
    }
}

void terminal_clear(Renderer *, APIParameter *) { clear(); }

void terminal_flush(Renderer *, APIParameter *) { refresh(); }

void terminal_autoresize(Renderer *r, APIParameter *) {
    TRenderer *tr = r->renderer;

    int w = 50, h = 50;
    getmaxyx((tr->win), h, w);

    r->cfg->width = w;
    r->cfg->height = h;
}

static DrawFunc *TERMINAL_DRAW_FUNC_MAP[] = {
    [renderapi_plot] = &terminal_draw_plot,
    [renderapi_rect_wh] = &terminal_draw_rect_wh,
    [renderapi_rect_xy] = &terminal_draw_rect_xy,
    [renderapi_clear] = &terminal_clear,
    [renderapi_flush] = &terminal_flush,
    [renderapi_color] = &terminal_set_color,
    [renderapi_fill] = &terminal_clear,
    [renderapi_resize] = &terminal_autoresize,
};

void terminal_renderer_init(Renderer *r) {
    TRenderer *tr = malloc(sizeof(TRenderer));
    assert(tr);

    setlocale(LC_ALL, "");
    printf("\x1B[?25l");

    tr->win = initscr();
    tr->ch = '*';

    r->renderer = tr;

    int val = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (val != -1) {
        fcntl(STDIN_FILENO, F_SETFL, val | O_NONBLOCK);
    }

    raw();
    noecho();
    keypad(tr->win, TRUE);

    r->api = TERMINAL_DRAW_FUNC_MAP;
}

void terminal_renderer_end(Renderer *r) {
    endwin();
    printf("\x1B[?25h");
    free(r->renderer);
    r->renderer = nullptr;
}
