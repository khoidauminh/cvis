#include "declare.h"

#include "fft.h"

#include <assert.h>
#include <complex.h>
#include <stdbit.h>
#include <stdlib.h>

#define MAX_FFT_POWER 13
#define MAX_FFT_LENGTH (1 << MAX_FFT_POWER)

static cplx *TWIDDLE_ARRAY = NULL;

void free_twiddle_array() {
    free(TWIDDLE_ARRAY);
    TWIDDLE_ARRAY = NULL;
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

void butterfly(cplx *arr, uint len, uint power) {
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

uint ulog2(uint x) {
    constexpr uint UINT_SIZE = 8 * sizeof(uint);
    return UINT_SIZE - stdc_leading_zeros(x >> 1);
}

void fft_inplace(cplx *const arr, const uint len) {
    if (TWIDDLE_ARRAY == NULL) {
        contruct_twiddle_array();
    }

    const uint power = ulog2(len);
    butterfly(arr, len, power);

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

void fft_inplace_stereo(cplx *arr, uint len, uint upto, bool normalize) {
    fft_inplace(arr, len);

    uint bound = SDL_min(len / 2, upto);

    for (uint i = 1; i < bound; i++) {
        cplx z1 = arr[i];
        cplx z2 = conjf(arr[len - i]);
        arr[i] = CMPLXF(l1norm(z1 + z2), l1norm(z1 - z2));
    }

    if (normalize) {
        float norm = 1.0f / (float)len;

        for (uint i = 0; i < bound; i++) {
            arr[i] *= norm;
        }
    }
}
