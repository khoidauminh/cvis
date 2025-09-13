#include "config.h"
#include "logging.h"
#include "program.h"

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <stdio.h>

Config config_default() {
    return (Config){
        .background = {40, 40, 40, 255},
        .refreshmode = CVIS_REFRESHMODE_SYNC,
        .width = 84,
        .height = 84,
        .scale = 2,
        .refreshrate = 60,
        .visname = "",
        .resizable = false,
    };
}

void config_print(const Config *cfg) {

    info("--- Config details ---\n");

    info("Size: %ux%u\n", cfg->width, cfg->height);

    if (cfg->refreshmode == CVIS_REFRESHMODE_SYNC) {
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

            if (cfg.width > MAX_LOGICAL_SIZE || cfg.height > MAX_LOGICAL_SIZE) {
                cfg.width = uint_min(cfg.width, MAX_LOGICAL_SIZE);
                cfg.height = uint_min(cfg.height, MAX_LOGICAL_SIZE);
                warn("Width and/or height clamped to the maximum allowed!\n");
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

            cfg.background = (Color){r, g, b, 255};

            continue;
        }

        if (!strcmp(*argv, "--fps")) {
            const char *strfps = argv_next(&argv, argvend);

            uint fps = 60;

            if (!sscanf(strfps, "%u", &fps)) {
                die("Invalid or missing value for fps.\n");
            }

            cfg.refreshmode = CVIS_REFRESHMODE_SET;

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

        if (!strcmp(*argv, "--resizable")) {
            cfg.resizable = true;
            continue;
        }

        warn("Unknown option %s.\n", *argv);
    }

    return cfg;
}
