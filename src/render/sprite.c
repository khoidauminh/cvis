#include "public/common.h"
#include "public/logging.h"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>

#include <assert.h>

typedef struct sprite {
    SDL_Texture *texture;
} Sprite;

Sprite sprite_new(const char *sprite_path, sColor key) {
    SDL_Surface *s = SDL_LoadBMP(sprite_path);
    if (!s) {
        goto ERROR;
    }

    const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(s->format);
    Uint32 k = SDL_MapRGB(format, nullptr, key.r, key.g, key.b);

    if (!SDL_SetSurfaceColorKey(s, true, k)) {
        warn("Failed to set sColor key: %s\n", SDL_GetError());
    }

    SDL_Renderer *r = SDL_CreateSoftwareRenderer(s);
    if (!r)
        goto ERROR;

    SDL_Texture *t = SDL_CreateTextureFromSurface(r, s);
    if (!t)
        goto ERROR;

    SDL_DestroyRenderer(r);
    SDL_DestroySurface(s);

    return (Sprite){.texture = t};

ERROR:
    die("%s\n", SDL_GetError());
}

void sprite_render(SDL_Renderer *r, const Sprite *sprite, float x, float y) {
    float w, h;
    if (!SDL_GetTextureSize(sprite->texture, &w, &h)) {
        goto ERROR;
    }

    SDL_FRect rect = {x, y, w, h};
    if (!SDL_RenderTexture(r, sprite->texture, &rect, &rect)) {
        goto ERROR;
    }

    return;
ERROR:
    die("%s\n", SDL_GetError());
}
