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
        RNDR_RECT(w / 2.0f, h / 10.0f, 1.0f, h - h / 5.0f + 1.0f);
    } else {
        RNDR_RECT(w / 10.0f, h / 2.0f, h - h / 5.0f + 1.0f, 1.0f);
    }
}

void visualizer_vectorscope(Program *prog) {
    cplx buffer[BUFFERSIZE];
    buffer_read(buffer, BUFFERSIZE);
    buffer_autoslide();

    RNDR_SET_TARGET(pg_renderer(prog));

    RNDR_CLEAR();

    Size size = RNDR_SIZE();

    float center_x = size.w / 2.0;
    float center_y = size.h / 2.0;

    float scale = (size.w < size.h ? size.w : size.h) * 0.4;

    for (uint i = 0; i < BUFFERSIZE; i++) {
        uint il = i;
        uint ir = (i + ROTATESIZE_DEFAULT) % BUFFERSIZE;

        float x = crealf(buffer[il]) * scale;
        float y = cimagf(buffer[ir]) * scale;

        float redf = (fabsf(x) + fabsf(y)) * 7.0;
        Uint8 red = SDL_min((int)(redf), 255);

        RNDR_COLOR(red, 255, 0, 255);
        RNDR_PLOT(center_x + x, center_y + y);
    }

    draw_cross(prog);

    RNDR_FLUSH();
}
