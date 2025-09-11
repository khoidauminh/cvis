#include "common.h"
#include "render-private.h" // IWYU pragma: keep.
#include "render.h"

#include <SDL3/SDL_stdinc.h>
#include <assert.h>
#include <stdlib.h>

#include <locale.h>
#include <ncurses.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <SDL3/SDL_timer.h>

#include <unistdio.h>

constexpr uint CHAR_MAP_LEN = 5;
constexpr chtype CHAR_MAP[CHAR_MAP_LEN] = {' ', '-', '+', '#', '@'};

typedef struct terminal_renderer {
    WINDOW *win;
    chtype ch;
} TRenderer;

static void terminal_set_color(Renderer *rndr, Color c) {
    TRenderer *tr = rndr->renderer;

    uint gray = ((uint)c.r + (uint)c.g + (uint)c.b * 2) * (uint)c.a / 256 / 4;
    uint index = gray * CHAR_MAP_LEN / 256;

    tr->ch = CHAR_MAP[index];
}

static void terminal_draw_plot(Renderer *r, float x, float y) {
    TRenderer *tr = r->renderer;

    mvwaddch(tr->win, (int)y + 1, (int)x + 1, tr->ch);
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

static void terminal_set_blendmode(Renderer *, uint) {}

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
        mvwaddch(tr->win, dy, dx, tr->ch);

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

static void terminal_text(Renderer *r, float x, float y, const char *str) {
    TRenderer *tr = r->renderer;
    mvwaddstr(tr->win, y, x, str);
}

static void terminal_fade(Renderer *, Uint8) {}

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
    .text = terminal_text,
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
    keypad(tr->win, true);

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
    PG_SET_TARGET(p);

    while (running) {
        pg_keymap_reset(p);

        const chtype key = getch();

        switch (key) {
        case 'q':
            running = false;
            break;
        case ' ':
            vm_next(pg_vismanager(p));
            break;

        case KEY_LEFT:
            pg_keymap_set(p, keyevents_left, true);
            break;

        case KEY_RIGHT:
            pg_keymap_set(p, keyevents_right, true);
            break;

        case KEY_UP:
            pg_keymap_set(p, keyevents_up, true);
            break;

        case KEY_DOWN:
            pg_keymap_set(p, keyevents_down, true);
            break;

        case 'c':
            pg_keymap_set(p, keyevents_c, true);
            break;

        case 'x':
            pg_keymap_set(p, keyevents_x, true);
            break;

        case 'z':
            pg_keymap_set(p, keyevents_z, true);
            break;

        default: {
        }
        }

        RNDR_AUTORESIZE();
        vm_perform(p);
        SDL_Delay(1000 / 60);
    }
}
