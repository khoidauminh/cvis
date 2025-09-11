#include "common.h"
#include "logging.h"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>

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
#embed "../../assets/charmap-cellphone.bmp"
};

typedef struct sdl_font_map {
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    SDL_Texture *texture;
} SDLFontMap;

static thread_local SDLFontMap *fontmap = nullptr;

static void deinit() {
    SDL_DestroySurface(fontmap->surface);
    SDL_DestroyTexture(fontmap->texture);
}

static void init(SDL_Renderer *renderer) {
    fontmap = malloc(sizeof(*fontmap));
    assert(fontmap);

    fontmap->renderer = renderer;

    fontmap->surface =
        SDL_LoadBMP_IO(SDL_IOFromConstMem(FONT_FILE, sizeof(FONT_FILE)), true);

    if (!fontmap->surface)
        goto ERROR;

    fontmap->texture = SDL_CreateTextureFromSurface(renderer, fontmap->surface);

    if (!fontmap->texture)
        goto ERROR;

    bool result =
        SDL_SetTextureScaleMode(fontmap->texture, SDL_SCALEMODE_NEAREST);

    if (!result)
        goto ERROR;

    atexit(deinit);
    return;

ERROR:
    die("%s\n", SDL_GetError());
}

void sdlfont_draw_char(SDL_Renderer *renderer, const char c, float x, float y) {
    if (c < '!' || c > '~') {
        return;
    }

    if (!fontmap) {
        init(renderer);
    }

    if (fontmap->renderer != renderer) {
        die("Different renderers!\n");
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

    bool result =
        SDL_RenderTexture(renderer, fontmap->texture, &srcrect, &dstrect);

    if (!result) {
        die("%s\n", SDL_GetError());
    }
}

void sdlfont_draw_line(SDL_Renderer *render, const char *strstart,
                       const char *strend, float x, float y,
                       TextAlignment align) {

    const ulong length = (ulong)(strend - strstart);

    const float drawwidth = (float)(length * CHAR_WIDTH);

    switch (align) {
    case CVIS_TEXTALIGN_MIDDLE:
        x -= drawwidth * 0.5f;
        break;
    case CVIS_TEXTALIGN_RIGHT:
        x -= drawwidth;
    default: {
    }
    }

    for (const char *i = strstart; i < strend; i++) {
        const char c = *i;
        sdlfont_draw_char(render, c, x, y);
        x += (float)CHAR_WIDTH;
    }
}

void sdlfont_draw_str(SDL_Renderer *renderer, const char *str, float x, float y,
                      TextAlignment align, TextAnchor anchor) {

    // Only count for lines when not anchoring top.
    if (anchor != CVIS_TEXTANCHOR_TOP) {
        // Counts the number of lines;
        uint stringheight = CHAR_WIDTH;
        for (const char *c = str; *c; c++) {
            if (*c == '\n')
                stringheight += CHAR_WIDTH;
        }

        switch (anchor) {
        case CVIS_TEXTANCHOR_BOTTOM:
            y -= (float)stringheight;
            break;
        case CVIS_TEXTANCHOR_MIDDLE:
            y -= (float)stringheight * 0.5f;
        default: {
        }
        }
    }

    const char *oldnewline = str;
    const char *newline = str;

    while (true) {
        while (*newline && *newline != '\n')
            newline++;

        sdlfont_draw_line(renderer, oldnewline, newline, x, y, align);

        if (!*newline)
            return;

        newline++;
        oldnewline = newline;

        y += (float)CHAR_HEIHT;
    }
}
