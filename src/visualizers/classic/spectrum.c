#include "audio.h"
#include "fft.h"
#include "interpolation.h"
#include "program.h"
#include "render.h"

#include <math.h>

#include "visualizer.h"

constexpr uint BUFFERSIZE = 256;
constexpr uint SPECTRUMSIZE = BUFFERSIZE >> 2;
constexpr float SMOOTHING = 0.8f;

static cplx fft[SPECTRUMSIZE] = {0.0f};

void prepare() {
    cplx buffer[BUFFERSIZE];
    buffer_read(buffer, BUFFERSIZE);
    buffer_rotate_left(BUFFERSIZE / 2);

    fft_inplace_stereo(buffer, BUFFERSIZE, SPECTRUMSIZE, true);

    for (uint i = 0; i < SPECTRUMSIZE; i++) {
        float scale = log2f((float)(i + 2));

        // float re = decay(crealf(fft[i]), fabsf(crealf(buffer[i])),
        // SMOOTHING); float im = decay(cimagf(fft[i]),
        // fabsf(cimagf(buffer[i])), SMOOTHING);
        fft[i] = clinearf(fft[i], quad1(buffer[i]) * scale, 0.2);
    }
}

void visualizer_spectrum(Program *prog) {
    prepare();

    RNDR_SET_TARGET(prog->renderer);

    RNDR_COLOR(20, 20, 20, 255);
    RNDR_FILL();

    Size size = RNDR_SIZE();

    render_set_color(prog->renderer, 255, 255, 255, 255);

    for (uint y = 0; y < size.h; y++) {
        uint i = y * SPECTRUMSIZE / size.h;
        cplx sample = fft[i] * size.w;
        float sl = crealf(sample);
        float sr = cimagf(sample);

        RNDR_RECT_WH(size.w / 2.0 - sl, size.h - y, sl, 1.0);
        RNDR_RECT_WH(size.w / 2.0, size.h - y, sr, 1.0);
    }

    RNDR_FLUSH();
}
