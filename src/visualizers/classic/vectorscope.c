#include "audio.h"
#include "program.h"
#include "render.h"
#include <math.h>

#include "visualizer.h"

constexpr uint BUFFERSIZE = 128;

void draw_cross(Program *prog) {
    static thread_local bool vertical = false;
    RNDR_SET_TARGET(pg_renderer(prog));
    RNDR_COLOR(44, 44, 44, 255);

    Size size = RNDR_SIZE();

    float w = size.w;
    float h = size.h;

    if (vertical ^= true) {
        RNDR_RECT(w * 0.5f, h * 0.1f, 1.0f, h * 0.8f + 1.0f);
    } else {
        RNDR_RECT(w * 0.1f, h * 0.5f, h * 0.8f + 1.0f, 1.0f);
    }
}

void visualizer_vectorscope(Program *prog) {
    cplx buffer[BUFFERSIZE];
    BUFFER_READ(buffer, BUFFERSIZE);
    BUFFER_AUTOSLIDE();

    RNDR_SET_TARGET(pg_renderer(prog));

    RNDR_CLEAR();

    Size size = RNDR_SIZE();

    float center_x = size.w / 2.0;
    float center_y = size.h / 2.0;

    float scale = (size.w < size.h ? size.w : size.h) * 0.4f;

    constexpr uint PHASE = BUFFERSIZE / 4;

    for (uint i = PHASE; i < BUFFERSIZE; i++) {
        uint il = i;
        uint ir = i - PHASE;

        float x = crealf(buffer[il]) * scale;
        float y = cimagf(buffer[ir]) * scale;

        float redf = (fabsf(x) + fabsf(y)) * 7.0f;
        Uint8 red = SDL_min((int)(redf), 255);

        RNDR_COLOR(red, 255, 0, 255);
        RNDR_PLOT(center_x + x, center_y + y);
    }

    draw_cross(prog);
}
