#include "audio.h"
#include "common.h"
#include "program.h"
#include "render.h"
#include <math.h>

#include "interpolation.h"
#include "visualizer.h"

#include <SDL3/SDL_render.h>

static constexpr uint BUFFERSIZE = 1024;
static constexpr uint PADDING = BUFFERSIZE;
static constexpr uint START = BUFFERSIZE / 2;
static constexpr uint PRESMOOTH = START / 8;
static constexpr float LOWPASS_FACTOR = 0.01f;
static constexpr uint SHIFTBACK = 50; // The lowpass causes a delay so shift
                                      // the index_start back (approxmiate).

static thread_local cplx BUFFER[BUFFERSIZE + PADDING];
static thread_local uint indexstart = 0;

static void prepare() {
    BUFFER_READ(BUFFER, BUFFERSIZE + PADDING);

    constexpr uint STORESIZE = 6;
    uint INDICES[STORESIZE] = {START};
    uint indices_last = 1;

    cplx smp1 = {}, smp2 = {}, smp3 = {};

    for (uint i = START - PRESMOOTH; i < START; i++) {
        smp3 = clinearf(smp3, BUFFER[i], LOWPASS_FACTOR);
        smp1 = smp2;
        smp2 = smp3;
    }

    for (uint i = START; i < PADDING + START; i++) {
        smp3 = clinearf(smp3, BUFFER[i], LOWPASS_FACTOR);

        if (crealf(smp1) >= 0.0f && crealf(smp3) < 0.0f) {
            INDICES[indices_last++] = i;
        }

        if (indices_last >= STORESIZE)
            break;

        if (cimagf(smp1) >= 0.0f && cimagf(smp3) < 0.0f) {
            INDICES[indices_last++] = i;
        }

        if (indices_last >= STORESIZE)
            break;

        smp1 = smp2;
        smp2 = smp3;
    }

    indexstart = INDICES[indices_last - 1] - START - SHIFTBACK;

    BUFFER_AUTOSLIDE();
}

void visualizer_oscilloscope(Program *prog) {
    prepare();

    RNDR_SET_TARGET(pg_renderer(prog));
    RNDR_CLEAR();
    RNDR_BLEND(SDL_BLENDMODE_ADD);

    Uint2D size = RNDR_SIZE();

    float center = (float)size.y * 0.5f;
    float scale = center * 0.7f;

    const uint w = uint_max(size.x, 1);

    const float buffer_size_smaller = (float)BUFFERSIZE * 0.8f;
    const float index_scale = buffer_size_smaller / (float)w;
    const uint base = (uint)((float)BUFFERSIZE * 0.1f);

    const uint samplesperpixel = (BUFFERSIZE + w) / w;

    for (uint x = 0; x < size.x; x++) {
        const uint istart = (uint)((float)x * index_scale) + indexstart + base;
        const uint iend = istart + samplesperpixel;

        float left_min = 100.f;
        float left_max = -100.f;

        float right_min = 100.f;
        float right_max = -100.f;

        for (uint i = istart; i < iend; i++) {
            uint j = i;

            float left = crealf(BUFFER[j]);
            float right = cimagf(BUFFER[j]);

            left_min = fminf(left, left_min);
            left_max = fmaxf(left, left_max);

            right_min = fminf(right, right_min);
            right_max = fmaxf(right, right_max);
        }

        left_min = left_min * scale + center;
        left_max = left_max * scale + center;

        right_min = right_min * scale + center;
        right_max = right_max * scale + center;

        RNDR_COLOR(0, 200, 55, 255);
        RNDR_RECT((float)x, left_min, 1.0f, left_max - left_min);

        RNDR_COLOR(0, 55, 200, 255);
        RNDR_RECT((float)x, right_min, 1.0f, right_max - right_min);
    }
}
