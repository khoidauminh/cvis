#include "audio.h"
#include "fft.h"
#include "interpolation.h"
#include "program.h"
#include "render.h"

#include <complex.h>
#include <math.h>

#include "visualizer.h"

constexpr uint BUFFERSIZE = 1 << 10;
constexpr uint READ_SIZE = BUFFERSIZE / 2;
constexpr uint SPECTRUMSIZE = 64;
constexpr float SMOOTHING = 0.91f;

static thread_local cplx fft[SPECTRUMSIZE + 1] = {};

void prepare() {
    static thread_local cplx buffer[BUFFERSIZE] = {0.0f};
    uint read = buffer_read(buffer, READ_SIZE);
    memset(buffer + read, 0, sizeof(cplx) * (BUFFERSIZE - read));
    buffer_autoslide();

    fft_inplace_stereo(buffer, BUFFERSIZE, SPECTRUMSIZE);
    fft_prettify(buffer, BUFFERSIZE, SPECTRUMSIZE);

    compress(buffer, SPECTRUMSIZE, 0.0f, 0.9f);

    for (uint i = 0; i < SPECTRUMSIZE; i++) {
        float re = decay(crealf(fft[i]), crealf(buffer[i]), SMOOTHING);
        float im = decay(cimagf(fft[i]), cimagf(buffer[i]), SMOOTHING);
        fft[i] = CMPLXF(re, im);
    }
}

void visualizer_spectrum(Program *prog) {
    prepare();

    RNDR_SET_TARGET(pg_renderer(prog));

    RNDR_CLEAR();

    Size size = RNDR_SIZE();

    for (uint y = 0; y < size.h; y++) {
        float ifrac = (float)y / (float)size.h;
        ifrac = exp2m1f(ifrac);
        float ifloat = ifrac * SPECTRUMSIZE;
        uint ifloor = (uint)ifloat;
        uint iceil = (uint)(ceilf(ifloat));
        float ti = ifloat - (float)ifloor;

        cplx sfloor = fft[ifloor];
        cplx sceil = fft[iceil];
        float sl = smooth_step(crealf(sfloor), crealf(sceil), ti);
        float sr = smooth_step(cimagf(sfloor), cimagf(sceil), ti);

        sl = powf(sl, 1.3f) * (float)(size.w) * 0.5f;
        sr = powf(sr, 1.3f) * (float)(size.w) * 0.5f;

        Uint8 channel = (Uint8)(y * 255 / size.h);
        Uint8 green = (Uint8)SDL_min(16 + (int)(3.0f * (sl + sr)), 255);

        RNDR_COLOR(255 - channel, green, 128 + channel / 2, 255);
        RNDR_RECT(size.w / 2.0 - sl, size.h - y, sl, 1.0);
        RNDR_RECT(size.w / 2.0, size.h - y, sr, 1.0);

        cplx s = *buffer_get(ifloor);
        Uint8 c1 = crealf(s) > 0.0 ? 255 : 0;
        Uint8 c2 = cimagf(s) > 0.0 ? 255 : 0;

        RNDR_COLOR(c1, 0, c2, 255);
        RNDR_RECT(size.w / 2.0 - 1, size.h - y, 2, 1);
    }

    RNDR_FLUSH();
}
