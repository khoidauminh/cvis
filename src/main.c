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
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_rect.h>

static Program *program = NULL;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **, int, char *[]) {
    SDL_SetAppMetadata("Example Renderer Clear", "1.0",
                       "com.example.renderer-clear");

    program = program_new(rt_sdl, 1000 / 144);
    program->renderer = renderer_init(rt_sdl, 90, 90);

    init_audio();

    return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS; /* end the program, reporting success to the OS.
                                 */
    }
    return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *) {
    // visualizer_vectorscope(program);
    visualizer_spectrum(program);

    // SDL_Delay(program->refreshrate);

    return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *, SDL_AppResult) {
    /* SDL will clean up the window/renderer for us. */

    renderer_end(program->renderer);
    free(program);

    free_audio();
}
