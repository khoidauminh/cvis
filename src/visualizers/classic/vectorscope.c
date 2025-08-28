#include "audio.h"
#include "program.h"
#include "render.h"
#include <math.h>

#include "visualizer.h"

#define BUFFERSIZE 128

void visualizer_vectorscope(Program *prog) {
    cplx buffer[BUFFERSIZE];
    buffer_read(buffer, BUFFERSIZE);
    buffer_autoslide();

    RNDR_SET_TARGET(pg_renderer(prog));

    RNDR_COLOR(0, 0, 0, 0);
    RNDR_FILL();

    Size size = RNDR_SIZE();

    float center_x = size.w / 2.0;
    float center_y = size.h / 2.0;

    for (uint i = 0; i < BUFFERSIZE; i++) {
        float x = crealf(buffer[i]) * size.w;
        float y = cimagf(buffer[i]) * size.h;
        float redf = fabsf(x) + fabsf(y);
        Uint8 red = SDL_min((int)(redf), 255);

        RNDR_COLOR(red, 255, 0, 255);
        RNDR_PLOT(center_x + x, center_y + y);
    }

    RNDR_FLUSH();
}
