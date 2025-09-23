#include "audio.h"
#include "common.h"
#include "render.h"
#include <math.h>

constexpr tUint BUFFERSIZE = 128;

void draw_cross() {
    static thread_local bool vertical = false;

    RNDR_COLOR((sColor){70, 70, 70, 255});

    sUint2d size = RNDR_SIZE();

    float w = (float)size.x;
    float h = (float)size.y;

    if (vertical ^= true) {
        RNDR_RECT(w * 0.5f, h * 0.1f, 1.0f, h * 0.8f + 1.0f);
    } else {
        RNDR_RECT(w * 0.1f, h * 0.5f, h * 0.8f + 1.0f, 1.0f);
    }
}

void visualizer_vectorscope() {
    if (BUFFER_QUIET()) {
        return;
    }

    tCplx BUFFER[BUFFERSIZE];
    BUFFER_READ(BUFFER, BUFFERSIZE);
    BUFFER_AUTOSLIDE();

    RNDR_CLEAR();

    sUint2d size = RNDR_SIZE();

    float center_x = (float)size.x / 2.0f;
    float center_y = (float)size.y / 2.0f;

    float scale = (float)(size.x < size.y ? size.x : size.y) * 0.4f;

    constexpr tUint PHASE = 92;

    for (tUint i = PHASE; i < BUFFERSIZE; i++) {
        tUint il = i;
        tUint ir = i - PHASE;

        float x = crealf(BUFFER[il]) * scale;
        float y = cimagf(BUFFER[ir]) * scale;

        float redf = (fabsf(x) + fabsf(y)) * 7.0f;
        tUbyte red = (tUbyte)uint_min((tUbyte)(redf), 255);

        RNDR_COLOR((sColor){red, 255, 0, 255});
        RNDR_PLOT(center_x + x, center_y + y);
    }

    draw_cross();
}
