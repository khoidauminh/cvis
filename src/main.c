#include "audio.h"
#include "program.h"
#include "render.h"
#include "visualizer.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>

int main() {
    init_audio();
    Program *program = pg_new(rt_sdl, 1000 / 144);

    pg_eventloop(program);

    pg_end(program);

    free_audio();
    SDL_Quit();
}
