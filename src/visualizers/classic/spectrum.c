#include "audio.h"
#include "fft.h"
#include "interpolation.h"
#include "program.h"
#include "render.h"

#include <complex.h>
#include <math.h>

#include "visualizer.h"

constexpr uint BUFFERSIZE = 1 << 9;
constexpr uint SPECTRUMSIZE = 64;
constexpr float SMOOTHING = 0.9f;

static cplx buffer[BUFFERSIZE] = {};
static cplx fft[SPECTRUMSIZE + 1] = {};

void prepare() {
    uint read_size = buffer_read(buffer, BUFFERSIZE);
    memset(buffer + read_size, 0, sizeof(cplx) * (BUFFERSIZE - read_size));
    buffer_slide(BUFFERSIZE / 4);

    fft_inplace_stereo(buffer, BUFFERSIZE, SPECTRUMSIZE, false);

    const float recip = 1.0 / SPECTRUMSIZE;

    for (uint i = 0; i < SPECTRUMSIZE; i++) {
        float scale = log2p1f((float)(i));
        buffer[i] = quad1(buffer[i]) * scale * recip;
    }

    normalize_max(buffer, SPECTRUMSIZE);

    for (uint i = 0; i < SPECTRUMSIZE; i++) {
        float re = decay(crealf(fft[i]), crealf(buffer[i]), SMOOTHING);
        float im = decay(cimagf(fft[i]), cimagf(buffer[i]), SMOOTHING);
        // fft[i] = clinearf(fft[i], buffer[i] * 0.02, 0.3);
        fft[i] = CMPLXF(re, im);
    }
}

void visualizer_spectrum(Program *prog) {
    prepare();

    RNDR_SET_TARGET(prog->renderer);

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

        sl = powf(sl, 1.2f) * (float)(size.w) * 0.5f * 0.9f;
        sr = powf(sr, 1.2f) * (float)(size.w) * 0.5f * 0.9f;

        Uint8 channel = (Uint8)(y * 255 / size.h);
        Uint8 green = (Uint8)SDL_min(16 + (int)(3.0f * (sl + sr)), 255);

        RNDR_COLOR(255 - channel, green, 128 + channel / 2, 255);
        RNDR_RECT_WH(size.w / 2.0 - sl, size.h - y, sl, 1.0);
        RNDR_RECT_WH(size.w / 2.0, size.h - y, sr, 1.0);

        cplx s = *buffer_get(ifloor);
        Uint8 c1 = crealf(s) > 0.0 ? 255 : 0;
        Uint8 c2 = cimagf(s) > 0.0 ? 255 : 0;

        RNDR_COLOR(c1, 0, c2, 255);
        RNDR_RECT_WH(size.w / 2.0 - 1, size.h - y, 2, 1);
    }

    RNDR_FLUSH();
}
