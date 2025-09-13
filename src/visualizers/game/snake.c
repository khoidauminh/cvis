#include "audio.h"
#include "common.h"
#include "program.h"

#include "render.h"

#include <SDL3/SDL_stdinc.h>
#include <assert.h>
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

constexpr uint INIT_CAP = 8;
constexpr uint APPLE_SCORE = 1;
constexpr uint SCALE = 2;

constexpr uint LOSE_SCREENTIME = 5;

constexpr uint RATE1 = 8;
constexpr uint RATE2 = 6;
constexpr uint RATE3 = 5;
constexpr uint RATE4 = 4;

constexpr uint SL2 = 10;
constexpr uint SL3 = 20;
constexpr uint SL4 = 30;

constexpr char LOSE_STR1[] = "YOU LOST!";
constexpr char LOSE_STR2[] = "PRETTY NICE!";
constexpr char LOSE_STR3[] = "WHAT\nTHE HECK?!";
constexpr char LOSE_STR4[] = "WHAT\nTHE FUCK?!!";

enum game_state {
    gs_running,
    gs_lose,
};

typedef struct snake {
    Uint2D *positions;
    uint len;
    uint cap;
    KeyEvent direction;
} Snake;

static void snake_free(Snake *sn) { free(sn->positions); }

static void snake_init(Snake *sn) {
    if (sn->positions) {
        sn->len = 1;
        return;
    }
    sn->positions = calloc(INIT_CAP, sizeof(Uint2D));
    assert(sn->positions != nullptr);
    sn->len = 1;
    sn->cap = INIT_CAP;
}

static void snake_grow(Snake *sn) {
    if (sn->len == sn->cap) {
        uint newcap = sn->cap * 2;
        Uint2D *newpositions = realloc(sn->positions, newcap * sizeof(Uint2D));
        assert(newpositions != nullptr);

        sn->positions = newpositions;
        sn->cap = newcap;
    }

    sn->len += 1;
}

static void snake_update(Snake *sn, Uint2D pos) {
    for (uint i = sn->len; i > 0; --i) {
        sn->positions[i] = sn->positions[i - 1];
    }

    sn->positions[0] = pos;
}

static bool snake_collision(Snake *sn) {

    const Uint2D head = sn->positions[0];

    for (uint i = 1; i < sn->len; i++) {
        const Uint2D body = sn->positions[i];

        if (body.x == head.x && body.y == head.y) {
            return true;
        }
    }

    return false;
}

static bool snake_at_apple(Snake *sn, Uint2D apple) {
    return sn->positions[0].x == apple.x && sn->positions[1].y == apple.y;
}

static void snake_move(Snake *sn, Uint2D bound) {
    Uint2D newpos = sn->positions[0];

    newpos.x += bound.x;
    newpos.y += bound.y;

    switch (sn->direction) {
    case KUP:
        newpos.y -= SCALE;
        break;
    case KLEFT:
        newpos.x -= SCALE;
        break;
    case KDOWN:
        newpos.y += SCALE;
        break;
    case KRIGHT:
        newpos.x += SCALE;
        break;
    default: {
    }
    }

    newpos.x %= bound.x;
    newpos.y %= bound.y;

    snake_update(sn, newpos);
}

typedef struct snake_game_state {
    bool init;
    Program *prog;
    Uint2D canvas;
    Snake snake;
    Uint2D apple;
    uint score;
    enum game_state state;

    ulong state_changed;

    ulong lose_screenframes;

    ulong age;

    ulong rate;
} SnakeGameState;

static thread_local SnakeGameState GAME = {};

static void reset_apple(SnakeGameState *game) {
    game->apple = (Uint2D){
        (((uint)rand() % game->canvas.x) / SCALE) * SCALE,
        ((uint)rand() % game->canvas.y / SCALE) * SCALE,
    };
}

static void game_init(SnakeGameState *game) {
    srand((uint)time(0));

    game->prog = PG_GET();

    game->lose_screenframes = PG_CONFIG()->refreshrate * LOSE_SCREENTIME;

    game->canvas = RNDR_SIZE();

    snake_init(&game->snake);

    reset_apple(game);

    game->score = 0;

    game->init = true;
    game->state = gs_running;
    game->rate = RATE1;
}

static void game_update(SnakeGameState *game) {
    assert(game->init);

    game->age += 1;

    game->canvas = RNDR_SIZE();

    if (game->state == gs_lose) {
        if (game->age - game->state_changed >= game->lose_screenframes) {
            game_init(game);
        } else {
            return;
        }
    }

    const uint map[] = {
        KLEFT,
        KUP,
        KRIGHT,
        KDOWN,
    };

    const KeyEvent oldd = game->snake.direction;

    for (uint i = 0; i < 4; i++) {
        if (pg_keymap_get(game->prog, map[i]) && map[(i + 2) % 4] != oldd) {
            game->snake.direction = map[i];
            break;
        }
    }

    if (snake_collision(&game->snake)) {
        game->state = gs_lose;
        game->state_changed = game->age;
    }

    if (snake_at_apple(&game->snake, game->apple)) {
        game->score += APPLE_SCORE;

        const uint s = game->score; // short name

        if (s < SL2) {
            snake_grow(&game->snake);
            game->rate = RATE1;
        } else if (s < SL3) {
            snake_grow(&game->snake);
            game->rate = RATE2;
        } else if (s < SL4) {
            snake_grow(&game->snake);
            game->rate = RATE3;
        } else if (s >= SL4) {
            snake_grow(&game->snake);
            game->rate = uint_max(RATE4 * s / SL4, 1);
        }

        reset_apple(game);
    }

    if (game->age % game->rate == 0) {
        snake_move(&game->snake, game->canvas);
    }
}

static void game_draw(SnakeGameState *game) {
    RNDR_CLEAR();

    // prints score
    constexpr uint scorestrlen = 20;
    char scorestr[scorestrlen];
    snprintf(scorestr, scorestrlen, "%u", game->score);

    RNDR_TEXT(0.0f, 0.0f, scorestr, CVIS_TEXTALIGN_LEFT, CVIS_TEXTANCHOR_TOP);

    // Make sure snake's color is always
    // different than background color.
    Color snakecolor = PG_CONFIG()->background;
    snakecolor.r += 128;
    snakecolor.g += 128;
    snakecolor.b += 128;

    const uint inputsize = BUFFER_INPUTSIZE();

    for (ulong i = 0; i < game->snake.len; i++) {
        Uint2D pos = game->snake.positions[i];

        float sumleft = 0.0f, sumright = 0.0f, sum = 0.0f;

        const uint readsize = inputsize / game->snake.len;
        const uint start = (uint)i * readsize;

        for (uint i = 0; i < readsize; i++) {
            sumleft += crealf(BUFFER_GET(i + start));
            sumright += cimagf(BUFFER_GET(i + start));
        }

        sum = sumleft + sumright;

        snakecolor.r += (Uint8)(sumleft);
        snakecolor.g += (Uint8)(sum);
        snakecolor.b += (Uint8)(sumright);

        RNDR_COLOR(snakecolor);

        RNDR_RECT((float)pos.x, (float)pos.y, (float)SCALE, (float)SCALE);
    }

    BUFFER_AUTOSLIDE();

    RNDR_COLOR((Color){255, 0, 0, 255});
    RNDR_RECT((float)game->apple.x, (float)game->apple.y, (float)SCALE,
              (float)SCALE);

    if (game->state == gs_lose) {
        RNDR_FADE(128);

        // Select lose screen text.
        const char *text = "";
        if (game->score < 30) {
            text = LOSE_STR1;
        } else if (game->score < 60) {
            text = LOSE_STR2;
        } else {
            if (game->age - game->state_changed < 40) {
                text = LOSE_STR4;
            } else {
                text = LOSE_STR3;
            }
        }

        RNDR_TEXT((float)(game->canvas.x) / 2, (float)game->canvas.y / 2, text,
                  CVIS_TEXTALIGN_MIDDLE, CVIS_TEXTANCHOR_MIDDLE);
    }
}

static void deinit() {
    snake_free(&GAME.snake);
    GAME.init = false;
}

void game_snake() {
    if (!GAME.init) {
        game_init(&GAME);
        atexit(deinit);
    }

    game_update(&GAME);
    game_draw(&GAME);
}
