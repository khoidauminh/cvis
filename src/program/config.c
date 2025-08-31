#include "config.h"
#include "logging.h"
#include "program.h"

#include <SDL3/SDL_pixels.h>
#include <stdio.h>

Config config_default() {
    return (Config){
        .background = {40, 40, 40, 255},
        .displaymode = displaymode_graphical,
        .refreshmode = refreshmode_sync,
        .width = 84,
        .height = 84,
        .scale = 2,
        .refreshrate = 60,
        .visname = "",
    };
}

RendererType displaymode_get_renderer(DisplayMode d) {
    switch (d) {
    case displaymode_graphical:
#ifdef USE_RAYLIB
        return renderertype_raylib;
#else
        return renderertype_sdl;
#endif
    case displaymode_terminal:
        return renderertype_terminal;
    default:
        die("Invalid display mode.");
    }

    return 0;
}

void config_print(const Config *cfg) {

    info("--- Config details ---\n");

    info("Display mode: ");
    switch (cfg->displaymode) {
    case displaymode_graphical:
        info("graphical\n");
        break;
    case displaymode_terminal:
        info("terminal\n");
        break;
    default: {
    }
    }

    info("Size: %ux%u\n", cfg->width, cfg->height);

    if (cfg->refreshmode == refreshmode_sync) {
        info("Vsync-ed\n");
    } else {
        info("Refresh rate: %u\n", cfg->refreshrate);
    }

    info("Scale: %u\n", cfg->scale);

    info("----------------------\n");
}

const char *argv_next(const char ***argv, const char **const end) {
    ++(*argv);

    if (*argv >= end) {
        die("Unexpected end of arguments.\n");
    }

    return **argv;
}

Config config_parse_args(const int argc, const char **argv) {
    Config cfg = config_default();

    const char **argvend = (argv++) + argc;

    for (; *argv; ++argv) {
        if (!strcmp(*argv, "--size")) {
            const char *strwidth = argv_next(&argv, argvend);

            if (!sscanf(strwidth, "%u", &cfg.width)) {
                die("Invalid or missing width for size.\n");
            }

            const char *strheight = argv_next(&argv, argvend);

            if (!sscanf(strheight, "%u", &cfg.height)) {
                die("Invalid or missing height for size.\n");
            }

            continue;
        }

        if (!strcmp(*argv, "--background")) {
            const char *strhex = argv_next(&argv, argvend);

            uint bg = 0;

            if (!strhex) {
                die("Invalid or missing value for background color.\n");
            }

            if (!sscanf(strhex, "%x", &bg)) {
                die("Invalid or missing value for background color.\n");
            }

            Uint8 r = (bg >> 16) & 0xFF;
            Uint8 g = (bg >> 8) & 0xFF;
            Uint8 b = (bg) & 0xFF;

            cfg.background = (SDL_Color){r, g, b, 255};

            continue;
        }

        if (!strcmp(*argv, "--fps")) {
            const char *strfps = argv_next(&argv, argvend);

            uint fps = 60;

            if (!sscanf(strfps, "%u", &fps)) {
                die("Invalid or missing value for fps.\n");
            }

            cfg.refreshmode = refreshmode_set;

            cfg.refreshrate = fps;

            if (cfg.refreshrate > REFRESHRATE_MAX) {
                warn("Maximum refresh rate allowed is %u.\n", REFRESHRATE_MAX);
                cfg.refreshrate = REFRESHRATE_MAX;
            }

            continue;
        }

        if (!strcmp(*argv, "--vis")) {
            cfg.visname = argv_next(&argv, argvend);
            continue;
        }

        if (!strcmp(*argv, "--terminal")) {
            cfg.displaymode = displaymode_terminal;
            continue;
        }

        warn("Unknown option %s.\n", *argv);
    }

    return cfg;
}
