#ifndef CVIS_INTERNAL_CONFIG_H
#define CVIS_INTERNAL_CONFIG_H

#include "public/config.h"

constexpr tUint MIN_PHYSICAL_SIZE = 64;
constexpr tUint MAX_LOGICAL_SIZE = 256;
constexpr SDL_RendererLogicalPresentation SCALE_MODE =
    SDL_LOGICAL_PRESENTATION_OVERSCAN;

sConfig config_default();
sConfig config_parse_args(const int argc, const char **argv);
void config_print(const sConfig *);

constexpr char CVIS_NAME[] = "cvis";
constexpr char CVIS_VERSION[] = "0.3.0";
constexpr char CVIS_AUTHOR[] = "khoidauminh";
constexpr char CVIS_LINK[] = "https://github.com/khoidauminh/cvis";

#endif
