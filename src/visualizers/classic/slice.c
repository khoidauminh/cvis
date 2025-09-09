#include "audio.h"
#include "common.h"
#include "interpolation.h"
#include "program.h"
#include "render.h"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_stdinc.h>
#include <math.h>
#include <threads.h>

static thread_local float ANGLE = 0.0f;
static thread_local float AMP = 0.0f;
static thread_local SDL_Color COLOR = {};

static void process_sweep(float *outsweep, float *outhigh) {
    const uint inputsize = BUFFER_INPUTSIZE();
    const float inputsizef = (float)inputsize;
    const float basslow = 1.0f / inputsizef * 0.5f;
    const float basehigh = 1.0f / inputsizef * 2.0f;

    const float treblelow = 1.0f / inputsizef * 50.f;
    const float treblehigh = 1.0f / inputsizef * 100.f;

    cplx high = 0.0f;
    cplx bin = 0.0f;

    for (uint i = 0; i < inputsize; i++) {
        const float fi = (float)i;
        const float t = fi / inputsizef;

        const cplx j = cexpf(fi * linearf(basslow, basehigh, t) * I);
        const cplx jhigh = cexpf(fi * linearf(treblelow, treblehigh, t) * I);

        bin += BUFFER_GET(i) * j;
        high += BUFFER_GET(i) * jhigh;
    }

    *outsweep = fminf(l1norm(bin) / inputsizef * 2.f, SDL_PI_F * 2.0f);
    *outhigh = l1norm(high);
}

void visualizer_slice(Program *prog) {
    RNDR_SET_TARGET(pg_renderer(prog));
    RNDR_FADE(10);

    Uint2D size = RNDR_SIZE();

    const uint radius = uint_min(size.x, size.y);
    const uint bigradius = radius / 5;
    const float bigradiusf = (float)bigradius;

    const float wf = (float)size.x / 2.0f;
    const float hf = (float)size.y / 2.0f;

    float sweep, high;
    process_sweep(&sweep, &high);

    const float amp =
        2.5f * fmaxf(sweep - AMP, 0.0f) + sweep * 0.3f + high * 0.00005f;

    const float d = 1.0f / (bigradiusf * SDL_PI_F + 1.0f);

    const Uint8 channel = (Uint8)uint_min((uint)high * 4, 255);

    COLOR.r += channel + 2;
    COLOR.g += channel;
    COLOR.b += channel + 5;

    RNDR_COLOR(COLOR.r, COLOR.g, COLOR.b, 255);

    float o = ANGLE;

    const float new_angle = ANGLE + amp;

    while (o < new_angle) {
        const float x = cosf(o) * bigradiusf * amp;
        const float y = sinf(o) * bigradiusf * amp;

        RNDR_PLOT(wf + x, hf + y);

        o += d;
    }

    ANGLE = fmodf(new_angle, SDL_PI_F * 2.0f);
}
