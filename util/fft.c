#include "common.h"

#include "fft.h"

#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdbit.h>
#include <stdlib.h>

#define MAX_FFT_POWER 13
#define MAX_FFT_LENGTH (1 << MAX_FFT_POWER)

static cplx *TWIDDLE_ARRAY = nullptr;

void free_twiddle_array() {
    free(TWIDDLE_ARRAY);
    TWIDDLE_ARRAY = nullptr;
}

void contruct_twiddle_array() {
    TWIDDLE_ARRAY = malloc(sizeof(cplx) * (MAX_FFT_LENGTH + 1));
    assert(TWIDDLE_ARRAY);

    uint i = 1;

    for (uint k = 1; k < MAX_FFT_LENGTH; k *= 2) {
        double angle = -SDL_PI_D / (double)k;

        for (uint j = 0; j < k; j++) {
            cplx twiddle = cexpf((double)j * angle * I);

            TWIDDLE_ARRAY[i++] = twiddle;
        }
    }

    atexit(free_twiddle_array);
}

uint reverse_bit(uint index, uint power) {
    uint out = 0;

    for (uint i = 0; i < power; i++) {
        out <<= 1;
        out |= index & 1;
        index >>= 1;
    }

    return out;
}

void butterfly_inplace(cplx *arr, uint len, uint power) {
    len -= 1;
    for (uint i = 1; i < len; i++) {
        uint ni = reverse_bit(i, power);
        if (ni > i) {
            cplx z = arr[i];
            arr[i] = arr[ni];
            arr[ni] = z;
        }
    }
}

void butterfly_io(cplx *in, cplx *out, uint len, uint power) {
    len -= 1;
    for (uint i = 1; i < len; i++) {
        uint ni = reverse_bit(i, power);
        out[ni] = in[i];
    }
}

uint ulog2(uint x) {
    constexpr uint UINT_SIZE = 8 * sizeof(uint);
    return UINT_SIZE - stdc_leading_zeros(x >> 1);
}

void compute_fft_inplace(cplx *const arr, const uint len) {
    if (TWIDDLE_ARRAY == nullptr) {
        contruct_twiddle_array();
    }

    for (uint half_window = 1; half_window < len; half_window *= 2) {
        const uint window = half_window * 2;
        cplx *root = TWIDDLE_ARRAY + half_window;

        for (uint k = 0; k < len; k += window) {
            cplx *slice_left = arr + k;
            cplx *slice_right = arr + k + half_window;

            for (uint j = 0; j < half_window; j++) {
                cplx z = slice_right[j] * root[j];
                slice_right[j] = slice_left[j] - z;
                slice_left[j] += z;
            }
        }
    }
}

void fft_inplace(cplx *arr, uint len) {
    const uint power = ulog2(len);
    butterfly_inplace(arr, len, power);
    compute_fft_inplace(arr, len);
}

void fft_inplace_stereo(cplx *arr, uint len, uint upto) {
    fft_inplace(arr, len);

    uint bound = uint_min(len / 2, upto);

    for (uint i = 1; i < bound; i++) {
        cplx z1 = arr[i];
        cplx z2 = conjf(arr[len - i]);
        arr[i] = CMPLXF(l1norm(z1 + z2), l1norm(z1 - z2));
    }
}

void fft_io_stereo(cplx *in, cplx *out, uint len, uint upto) {
    const uint power = ulog2(len);
    butterfly_io(in, out, len, power);
    fft_inplace_stereo(out, len, upto);
}

void fft_prettify(cplx *arr, const uint originallen, const uint upto) {
    const uint bound = uint_min(originallen / 2, upto);
    const float normalize = 2.f / (float)(originallen);

    for (uint i = 0; i < bound; i++) {
        arr[i] *= (log2f((float)(i + 1)) * normalize);
    }
}
