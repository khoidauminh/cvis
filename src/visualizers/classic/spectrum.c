#include "audio.h"
#include "common.h"
#include "fft.h"
#include "interpolation.h"
#include "render.h"

#include <complex.h>
#include <math.h>

constexpr uint BUFFERSIZE = 1 << 10;
constexpr uint READ_SIZE = BUFFERSIZE / 2;
constexpr uint SPECTRUMSIZE = 64;
constexpr float SMOOTHING = 0.91f;

static thread_local cplx fft[SPECTRUMSIZE + 1] = {};

static void prepare() {
    static thread_local cplx buffer[BUFFERSIZE] = {0.0f};
    uint read = BUFFER_READ(buffer, READ_SIZE);
    cplxzero(buffer + read, BUFFERSIZE - read);
    BUFFER_AUTOSLIDE();

    fft_inplace_stereo(buffer, BUFFERSIZE, SPECTRUMSIZE);
    fft_prettify(buffer, BUFFERSIZE, SPECTRUMSIZE);

    compress(buffer, SPECTRUMSIZE, 0.0f, 0.9f);

    for (uint i = 0; i < SPECTRUMSIZE; i++) {
        float re = decay(crealf(fft[i]), crealf(buffer[i]), SMOOTHING);
        float im = decay(cimagf(fft[i]), cimagf(buffer[i]), SMOOTHING);
        fft[i] = CMPLXF(re, im);
    }
}

void visualizer_spectrum() {
    prepare();

    RNDR_CLEAR();

    Uint2D size = RNDR_SIZE();

    float wf = (float)size.x;
    float hf = (float)size.y;

    for (uint y = 0; y < size.y; y++) {
        float ifrac = (float)y / (float)size.y;
        ifrac = exp2m1f(ifrac);
        float ifloat = ifrac * SPECTRUMSIZE;
        uint ifloor = (uint)ifloat;
        uint iceil = (uint)(ceilf(ifloat));
        float ti = ifloat - (float)ifloor;

        cplx sfloor = fft[ifloor];
        cplx sceil = fft[iceil];
        float sl = smooth_step(crealf(sfloor), crealf(sceil), ti);
        float sr = smooth_step(cimagf(sfloor), cimagf(sceil), ti);

        sl = powf(sl, 1.3f) * (float)(size.x) * 0.5f;
        sr = powf(sr, 1.3f) * (float)(size.x) * 0.5f;

        ubyte channel = (ubyte)(y * 255 / size.y);
        ubyte green = (ubyte)SDL_min(16 + (int)(3.0f * (sl + sr)), 255);

        float yf = (float)y;

        RNDR_COLOR((Color){255 - channel, green, 128 + channel / 2, 255});
        RNDR_RECT(wf / 2.0f - sl, hf - yf, sl, 1.0f);
        RNDR_RECT(wf / 2.0f, hf - yf, sr, 1.0f);

        cplx s = BUFFER_GET(size.y - y);
        ubyte c1 = crealf(s) > 0.0 ? 255 : 0;
        ubyte c2 = cimagf(s) > 0.0 ? 255 : 0;

        RNDR_COLOR((Color){c1, 0, c2, 255});
        RNDR_RECT(wf / 2.0f - 1, hf - yf, 2, 1);
    }
}
