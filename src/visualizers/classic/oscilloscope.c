#include "audio.h"
#include "program.h"
#include "render.h"
#include <math.h>

#include "interpolation.h"
#include "visualizer.h"

#include <SDL3/SDL_render.h>

constexpr uint BUFFERSIZE = 1024;
constexpr uint PADDING = BUFFERSIZE;
constexpr uint START = BUFFERSIZE / 2;

static thread_local cplx BUFFER[BUFFERSIZE + PADDING];
static thread_local uint indexstart = 0;

static void prepare() {
    BUFFER_READ(BUFFER, BUFFERSIZE + PADDING);

    constexpr uint STORESIZE = 6;
    uint INDICES[STORESIZE] = {START};
    uint indices_last = 1;

    cplx smp1 = BUFFER[START];
    cplx smp2 = clinearf(smp1, BUFFER[START + 1], 0.01f);
    cplx smp3 = clinearf(smp2, BUFFER[START + 2], 0.01f);

    for (uint i = START; i < PADDING + START; i++) {
        smp3 = clinearf(smp2, BUFFER[i], 0.01f);

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

    indexstart = INDICES[indices_last - 1] - START;

    BUFFER_SLIDE(indexstart / 2);
}

void visualizer_oscilloscope(Program *prog) {
    prepare();

    RNDR_SET_TARGET(pg_renderer(prog));
    RNDR_CLEAR();
    RNDR_BLEND(SDL_BLENDMODE_ADD);

    Size size = RNDR_SIZE();

    float center = (float)size.h * 0.5f;
    float scale = center * 0.7f;

    const uint w = uint_max(size.w, 1);

    const uint samplesperpixel = (BUFFERSIZE + w) / w;

    for (uint x = 0; x < size.w; x++) {
        const uint istart = x * BUFFERSIZE / w;
        const uint iend = istart + samplesperpixel;

        float left_min = 100.f;
        float left_max = -100.f;

        float right_min = 100.f;
        float right_max = -100.f;

        for (uint i = istart; i < iend; i++) {
            uint j = i + indexstart;

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
