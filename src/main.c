#include "config.h"

#include "program.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>

void init_audio();
void free_audio();

void pg_eventloop(Program *);
Program *pg_new(Config);

int main(int argc, const char *argv[]) {
    init_audio();

    Config cfg = config_parse_args(argc, argv);
    config_print(&cfg);

    Program *program = pg_new(cfg);

    pg_eventloop(program);

    pg_end(program);

    free_audio();
    SDL_Quit();
}
