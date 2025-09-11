#include "common.h"
#include "logging.h"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>
#include <assert.h>
#include <stdlib.h>
#include <threads.h>

constexpr uint SPRITESHEET_WIDTH = 128;
constexpr uint SPRITESHEET_HEIGHT = 64;

constexpr uint SPRITESHEET_COLUMNS = 18;
constexpr uint SPRITESHEET_ROWS = 7;

constexpr uint CHAR_WIDTH = SPRITESHEET_WIDTH / SPRITESHEET_COLUMNS;
constexpr uint CHAR_HEIHT = SPRITESHEET_HEIGHT / SPRITESHEET_ROWS;

static const uchar FONT_FILE[] = {
#embed "../../assets/charmap-cellphone_white_0.png"
};

typedef struct sdl_font_map {
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    SDL_Texture *texture;
} SDLFontMap;

static thread_local SDLFontMap *fontmap = nullptr;

static void deinit() {
    SDL_DestroyRenderer(fontmap->renderer);
    SDL_DestroySurface(fontmap->surface);
    SDL_DestroyTexture(fontmap->texture);
}

static void init() {
    fontmap = malloc(sizeof(*fontmap));
    assert(fontmap);

    fontmap->surface = SDL_CreateSurface(SPRITESHEET_WIDTH, SPRITESHEET_HEIGHT,
                                         SDL_PIXELFORMAT_ARGB8888);
    assert(fontmap->surface);

    fontmap->renderer = SDL_CreateSoftwareRenderer(fontmap->surface);
    assert(fontmap->renderer);

    fontmap->texture = IMG_LoadTexture_IO(
        fontmap->renderer, SDL_IOFromConstMem(FONT_FILE, sizeof(FONT_FILE)),
        true);

    assert(fontmap->texture);

    bool result = SDL_RenderTexture(fontmap->renderer, fontmap->texture,
                                    nullptr, nullptr);
    if (!result) {
        die("%s\n", SDL_GetError());
    }

    atexit(deinit);
}

void sdlfont_draw_char(SDL_Renderer *renderer, const char c, float x, float y) {
    if (c < '!' || c > '~') {
        return;
    }

    if (!fontmap) {
        init();
    }

    const uint index_flat = (uint)(c - ('!' - 1));
    const uint index_x = index_flat % SPRITESHEET_COLUMNS;
    const uint index_y = index_flat / SPRITESHEET_COLUMNS;

    const uint bitmapx = SPRITESHEET_WIDTH * index_x / SPRITESHEET_COLUMNS;
    const uint bitmapy = SPRITESHEET_HEIGHT * index_y / SPRITESHEET_ROWS;

    const SDL_FRect srcrect = {
        .x = (float)bitmapx,
        .y = (float)bitmapy,
        .w = (float)CHAR_WIDTH,
        .h = (float)CHAR_HEIHT,
    };

    const SDL_FRect dstrect = {
        .x = x,
        .y = y,
        .w = (float)CHAR_WIDTH,
        .h = (float)CHAR_HEIHT,
    };
}

void sdlfont_draw_str(SDL_Renderer *render, const char *str, float x, float y) {
    for (const char *i = str; *i; i++) {
        const char c = *i;
        sdlfont_draw_char(render, c, x, y);
        x += (float)CHAR_WIDTH;
    }
}
