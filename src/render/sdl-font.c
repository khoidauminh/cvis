#include "common.h"
#include "logging.h"

#include <SDL3/SDL_surface.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <assert.h>
#include <iso646.h>
#include <stdatomic.h>
#include <stdlib.h>

typedef struct sdl_font_manager {
    bool init;

    TTF_Font *font;
    SDL_Texture *texture;
    SDL_Surface *text;
    Color color;

    uchar *tiny_ttf;
    uint tiny_ttf_len;
} SDLFont;

static atomic_bool sdl_init = false;

void sfm_init(SDLFont *sdlf) {
    assert(!sdlf->init);

    if (!atomic_load(&sdl_init)) {
        if (!TTF_Init()) {
            die("Failed to initialize SDL font system.\n");
        }
        atexit(TTF_Quit);
        atomic_store(&sdl_init, true);
    }

    sdlf->color = (Color){255, 255, 255, 255};

    sdlf->font = TTF_OpenFontIO(
        SDL_IOFromConstMem(sdlf->tiny_ttf, sdlf->tiny_ttf_len), true, 10.0f);

    assert(sdlf->font);

    sdlf->init = true;
}

void sfm_render(SDLFont *sdlf, const char *str) {
    assert(sdlf && sdlf->init);

    sdlf->text = TTF_RenderText_Blended(sdlf->font, str, 0, sdlf->color);

    assert(sdlf->text);
}
