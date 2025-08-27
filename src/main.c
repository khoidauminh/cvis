#include "audio.h"
#include "program.h"
#include "render.h"
#include "visualizer.h"

#include <assert.h>
#include <stdlib.h>

#define SIZE 512
#define PRINT_SIZE 64

#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>

static Program *program = NULL;

int main() {
    program = program_new(rt_sdl, 1000 / 144);
    program->renderer =
        renderer_init(rt_sdl, DEFAULT_WIN_SIZE, DEFAULT_WIN_SIZE);

    init_audio();

    bool running = true;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) { // poll until all events are handled!
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
                break;
            }
        }

        visualizer_spectrum(program);

        // update game state, draw the current frame
    }

    renderer_end(program->renderer);
    free(program);

    free_audio();
    SDL_Quit();
}
