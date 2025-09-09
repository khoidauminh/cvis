#include "audio.h"
#include "common.h"
#include "program.h"
#include "render.h"
#include <math.h>

#include "visualizer.h"

constexpr uint BUFFERSIZE = 128;

void draw_cross(Program *prog) {
    static thread_local bool vertical = false;
    RNDR_SET_TARGET(pg_renderer(prog));
    RNDR_COLOR(70, 70, 70, 255);

    Uint2D size = RNDR_SIZE();

    float w = (float)size.x;
    float h = (float)size.y;

    if (vertical ^= true) {
        RNDR_RECT(w * 0.5f, h * 0.1f, 1.0f, h * 0.8f + 1.0f);
    } else {
        RNDR_RECT(w * 0.1f, h * 0.5f, h * 0.8f + 1.0f, 1.0f);
    }
}

void visualizer_vectorscope(Program *prog) {
    cplx BUFFER[BUFFERSIZE];
    BUFFER_READ(BUFFER, BUFFERSIZE);
    BUFFER_AUTOSLIDE();

    RNDR_SET_TARGET(pg_renderer(prog));

    RNDR_CLEAR();

    Uint2D size = RNDR_SIZE();

    float center_x = (float)size.x / 2.0f;
    float center_y = (float)size.y / 2.0f;

    float scale = (float)(size.x < size.y ? size.x : size.y) * 0.4f;

    constexpr uint PHASE = 92;

    for (uint i = PHASE; i < BUFFERSIZE; i++) {
        uint il = i;
        uint ir = i - PHASE;

        float x = crealf(BUFFER[il]) * scale;
        float y = cimagf(BUFFER[ir]) * scale;

        float redf = (fabsf(x) + fabsf(y)) * 7.0f;
        Uint8 red = (Uint8)uint_min((Uint8)(redf), 255);

        RNDR_COLOR(red, 255, 0, 255);
        RNDR_PLOT(center_x + x, center_y + y);
    }

    draw_cross(prog);
}
