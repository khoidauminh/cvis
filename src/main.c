#include "audio.h"
#include "program.h"
#include "render.h"
#include "visualizer.h"

#include <SDL3/SDL.h>

static Program *program = NULL;

int main() {
    program = pg_new(rt_sdl, 1000 / 144);
    pg_attach_renderer(
        program, renderer_init(rt_sdl, DEFAULT_WIN_SIZE, DEFAULT_WIN_SIZE));

    init_audio();

    bool running = true;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
                break;
            }
        }

        visualizer_spectrum(program);
    }

    renderer_end(pg_renderer(program));
    pg_end(program);

    free_audio();
    SDL_Quit();
}
