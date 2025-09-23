#include "audio.h"
#include "common.h"
#include "program.h"

#include "render.h"

#include <assert.h>
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

constexpr tUint INIT_CAP = 8;
constexpr tUint APPLE_SCORE = 1;
constexpr tUint SCALE = 2;

constexpr tUint LOSE_SCREENTIME = 3;

constexpr tUint RATE1 = 8;
constexpr tUint RATE2 = 6;
constexpr tUint RATE3 = 5;
constexpr tUint RATE4 = 4;

constexpr tUint SL2 = 10;
constexpr tUint SL3 = 20;
constexpr tUint SL4 = 30;

constexpr char LOSE_STR1[] = "YOU LOST!";
constexpr char LOSE_STR2[] = "PRETTY NICE!";
constexpr char LOSE_STR3[] = "WHAT\nTHE HECK?!";
constexpr char LOSE_STR4[] = "WHAT\nTHE FUCK?!!";

enum game_state {
    gs_running,
    gs_lose,
};

typedef struct snake {
    sUint2d *positions;
    tUint len;
    tUint cap;
    eKeyEvent direction;
} Snake;

static void snake_free(Snake *sn) { free(sn->positions); }

static void snake_init(Snake *sn) {
    if (sn->positions) {
        sn->len = 1;
        return;
    }
    sn->positions = calloc(INIT_CAP, sizeof(sUint2d));
    assert(sn->positions != nullptr);
    sn->len = 1;
    sn->cap = INIT_CAP;
}

static void snake_grow(Snake *sn) {
    if (sn->len == sn->cap) {
        tUint newcap = sn->cap * 2;
        sUint2d *newpositions =
            realloc(sn->positions, newcap * sizeof(sUint2d));
        assert(newpositions != nullptr);

        sn->positions = newpositions;
        sn->cap = newcap;
    }

    sn->len += 1;
}

static void snake_update(Snake *sn, sUint2d pos) {
    for (tUint i = sn->len; i > 0; --i) {
        sn->positions[i] = sn->positions[i - 1];
    }

    sn->positions[0] = pos;
}

static bool snake_collision(Snake *sn) {

    const sUint2d head = sn->positions[0];

    for (tUint i = 1; i < sn->len; i++) {
        const sUint2d body = sn->positions[i];

        if (body.x == head.x && body.y == head.y) {
            return true;
        }
    }

    return false;
}

static bool snake_at_apple(Snake *sn, sUint2d apple) {
    return sn->positions[0].x == apple.x && sn->positions[1].y == apple.y;
}

static void snake_move(Snake *sn, sUint2d bound) {
    sUint2d newpos = sn->positions[0];

    newpos.x += bound.x;
    newpos.y += bound.y;

    switch (sn->direction) {
    case KEYEVENT_UP:
        newpos.y -= SCALE;
        break;
    case KEYEVENT_LEFT:
        newpos.x -= SCALE;
        break;
    case KEYEVENT_DOWN:
        newpos.y += SCALE;
        break;
    case KEYEVENT_RIGHT:
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

    bool awaitinput;

    sUint2d canvas;
    Snake snake;
    sUint2d apple;
    tUint score;
    enum game_state state;

    tUlong state_changed;

    tUlong lose_screenframes;

    tUlong age;

    tUlong rate;
} SnakeGameState;

static thread_local SnakeGameState GAME = {};

static void reset_apple(SnakeGameState *game) {
    game->apple = (sUint2d){
        (((tUint)rand() % game->canvas.x) / SCALE) * SCALE,
        ((tUint)rand() % game->canvas.y / SCALE) * SCALE,
    };
}

static void game_init(SnakeGameState *game) {
    srand((tUint)time(0));

    game->lose_screenframes = PG_CONFIG()->refreshrate * LOSE_SCREENTIME;

    game->canvas = RNDR_SIZE();

    snake_init(&game->snake);

    reset_apple(game);

    game->score = 0;

    game->awaitinput = true;
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

    if (game->awaitinput) {
        const tUint map[] = {
            KEYEVENT_LEFT,
            KEYEVENT_UP,
            KEYEVENT_RIGHT,
            KEYEVENT_DOWN,
        };

        const eKeyEvent oldd = game->snake.direction;

        for (tUint i = 0; i < 4; i++) {
            if (PG_KEYPRESSED(map[i]) && map[(i + 2) % 4] != oldd) {
                game->snake.direction = map[i];
                break;
            }
        }

        game->awaitinput = false;
    }

    if (snake_collision(&game->snake)) {
        game->state = gs_lose;
        game->state_changed = game->age;
    }

    if (snake_at_apple(&game->snake, game->apple)) {
        game->score += APPLE_SCORE;

        const tUint s = game->score; // short name

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
            game->rate = uint_max(RATE4 * SL4 / s, 1);
        }

        reset_apple(game);
    }

    if (game->age % game->rate == 0) {
        snake_move(&game->snake, game->canvas);
        game->awaitinput = true;
    }
}

static void game_draw(SnakeGameState *game) {
    RNDR_CLEAR();

    // prints score
    constexpr tUint scorestrlen = 20;
    char scorestr[scorestrlen];
    snprintf(scorestr, scorestrlen, "%u", game->score);

    RNDR_TEXT(0.0f, 0.0f, scorestr, CVIS_TEXTALIGN_LEFT, CVIS_TEXTANCHOR_TOP);

    // Make sure snake's sColor is always
    // different than background sColor.
    sColor snakecolor = PG_CONFIG()->background;
    snakecolor.r += 128;
    snakecolor.g += 128;
    snakecolor.b += 128;

    const tUint inputsize = BUFFER_INPUTSIZE();

    for (tUlong i = 0; i < game->snake.len; i++) {
        sUint2d pos = game->snake.positions[i];

        float sumleft = 0.0f, sumright = 0.0f, sum = 0.0f;

        const tUint readsize = inputsize / game->snake.len;
        const tUint start = (tUint)i * readsize;

        for (tUint i = 0; i < readsize; i++) {
            sumleft += crealf(BUFFER_GET(i + start));
            sumright += cimagf(BUFFER_GET(i + start));
        }

        sum = sumleft + sumright;

        snakecolor.r += (tUbyte)(sumleft);
        snakecolor.g += (tUbyte)(sum);
        snakecolor.b += (tUbyte)(sumright);

        RNDR_COLOR(snakecolor);

        RNDR_RECT((float)pos.x, (float)pos.y, (float)SCALE, (float)SCALE);
    }

    BUFFER_AUTOSLIDE();

    RNDR_COLOR((sColor){255, 0, 0, 255});
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
