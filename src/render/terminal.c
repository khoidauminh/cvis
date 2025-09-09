#include "common.h"
#include "render.h"
#include "renderer-private.h" // IWYU pragma: keep.

#include <SDL3/SDL_stdinc.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <locale.h>
#include <ncurses.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <SDL3/SDL_timer.h>

#include <unistdio.h>

constexpr char CHAR_MAP[] = " -+#@";

typedef struct terminal_renderer {
    WINDOW *win;
    char ch;
} TRenderer;

static void terminal_set_color(Renderer *rndr, Uint8 r, Uint8 g, Uint8 b,
                               Uint8 a) {
    TRenderer *tr = rndr->renderer;
    const uint ch_map_size = (uint)strlen(CHAR_MAP);
    uint gray = ((uint)r + (uint)g + (uint)b * 2) * (uint)a / 256 / 4;
    uint index = gray * ch_map_size / 256;

    tr->ch = CHAR_MAP[index];
}

static void terminal_draw_plot(Renderer *r, float x, float y) {
    TRenderer *tr = r->renderer;

    mvwaddch(tr->win, (int)y + 1, (int)x + 1, (chtype)tr->ch);
}

static void terminal_draw_rect(Renderer *r, float xf, float yf, float wf,
                               float hf) {
    TRenderer *tr = r->renderer;

    int startx = (int)xf;
    int starty = (int)yf;
    int w = (int)wf;
    int h = (int)hf;

    int ww = (int)r->cfg->width;
    int wh = (int)r->cfg->height;

    if (startx < 0) {
        w += startx;
    }

    if (w + startx >= ww) {
        w = ww - startx;
    }

    if (starty < 0) {
        h += starty;
    }

    if (h + starty >= wh) {
        h = wh - starty;
    }

    if (w <= 0 || h <= 0) {
        return;
    }

    for (int row = 0; row < h; row++) {
        wmove(tr->win, starty + row, startx);

        for (int j = 0; j < w; j++) {
            waddch(tr->win, (chtype)tr->ch);
        }
    }
}

static void terminal_set_blendmode(Renderer *, SDL_BlendMode) {}

static void terminal_clear(Renderer *) { clear(); }

static void terminal_fill(Renderer *r) {
    terminal_draw_rect(r, 0.0f, 0.0f, (float)(r->cfg->width),
                       (float)(r->cfg->height));
}

static void terminal_line(Renderer *r, float x1f, float y1f, float x2f,
                          float y2f) {
    int x0 = (int)x1f;
    int y0 = (int)y1f;

    const int x1 = (int)x2f;
    const int y1 = (int)y2f;

    const int dx = x1 > x0 ? x1 - x0 : x0 - x1;
    const int sx = x0 < x1 ? 1 : -1;
    const int dy = y1 < y0 ? y1 - y0 : y0 - y1;
    const int sy = y0 < y1 ? 1 : -1;

    int error = dx + dy;

    TRenderer *tr = r->renderer;

    while (true) {
        mvwaddch(tr->win, dy, dx, (chtype)tr->ch);

        const int e2 = 2 * error;

        if (e2 >= dy) {
            if (x0 == x1)
                break;

            error += dy;
            x0 += sx;
        }

        if (e2 <= dx) {
            if (y0 == y1)
                break;

            error += dx;
            y0 += sy;
        }
    }
}

static void terminal_fade(Renderer *, Uint8 a) {}

static void terminal_flush(Renderer *) { refresh(); }

static void terminal_autoresize(Renderer *r) {
    TRenderer *tr = r->renderer;

    int w = 50, h = 50;
    getmaxyx((tr->win), h, w);

    r->cfg->width = (uint)w;
    r->cfg->height = (uint)h;
}

static const RenderVTable TERMINAL_VTABLE = {
    .blend = terminal_set_blendmode,
    .clear = terminal_clear,
    .color = terminal_set_color,
    .fill = terminal_fill,
    .line = terminal_line,
    .fade = terminal_fade,
    .flush = terminal_flush,
    .plot = terminal_draw_plot,
    .rect = terminal_draw_rect,
    .resize = terminal_autoresize,
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

    r->vtable = &TERMINAL_VTABLE;
}

void terminal_renderer_end(Renderer *r) {
    endwin();
    printf("\x1B[?25h");
    free(r->renderer);
    r->renderer = nullptr;
}

#include "program.h"

void pg_eventloop_term(Program *p) {
    assert(renderer_get_type(pg_renderer(p)) == renderertype_terminal);

    bool running = true;
    RNDR_SET_TARGET(pg_renderer(p));

    while (running) {
        const char key = (char)getch();

        switch (key) {
        case 'q':
            running = false;
            break;
        case ' ':
            vm_next(pg_vismanager(p));
            break;
        default: {
        }
        }

        RNDR_AUTORESIZE();
        vm_perform(p);
        SDL_Delay(1000 / 60);
    }
}
