#include "common.h"

#include "fft.h"

#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdbit.h>
#include <stdlib.h>

constexpr uint MAX_FFT_POWER = 13;
constexpr uint MAX_FFT_LENGTH = 1 << MAX_FFT_POWER;

static cplx *TWIDDLE_ARRAY = nullptr;
static uint *BUTTERFLY_ARRAY = nullptr;

static void free_twiddle_array() {
    free(TWIDDLE_ARRAY);
    TWIDDLE_ARRAY = nullptr;
}

static void contruct_twiddle_array() {
    TWIDDLE_ARRAY = malloc(sizeof(cplx) * (MAX_FFT_LENGTH + 1));
    assert(TWIDDLE_ARRAY);

    uint i = 1;

    for (uint k = 1; k < MAX_FFT_LENGTH; k *= 2) {
        float angle = -PI / (float)k;

        for (uint j = 0; j < k; j++) {
            cplx twiddle = cexpf((float)j * angle * I);

            TWIDDLE_ARRAY[i++] = twiddle;
        }
    }

    atexit(free_twiddle_array);
}

static uint reverse_bit(uint index, uint power) {
    uint out = 0;

    for (uint i = 0; i < power; i++) {
        out <<= 1;
        out |= index & 1;
        index >>= 1;
    }

    return out;
}

static void free_butterfly_array() {
    free(BUTTERFLY_ARRAY);
    BUTTERFLY_ARRAY = nullptr;
}

static void construct_butterfly_array() {
    BUTTERFLY_ARRAY = malloc(sizeof(uint) * MAX_FFT_LENGTH);
    assert(BUTTERFLY_ARRAY);

    for (uint power = 0; power < MAX_FFT_POWER; power++) {
        uint k = 1 << power;
        uint *arr = BUTTERFLY_ARRAY + k;

        for (uint j = 0; j < k; j++) {
            arr[j] = reverse_bit(j, power);
        }
    }

    atexit(free_butterfly_array);
}

static void butterfly_inplace(cplx *arr, uint len) {
    if (BUTTERFLY_ARRAY == nullptr) {
        construct_butterfly_array();
    }

    const uint *butterfy = BUTTERFLY_ARRAY + len;

    len -= 1;

    for (uint i = 1; i < len; i++) {
        uint ni = butterfy[i];
        if (ni > i) {
            cplx z = arr[i];
            arr[i] = arr[ni];
            arr[ni] = z;
        }
    }
}

uint ulog2(uint x) { return stdc_bit_width(x >> 1); }

static void compute_fft_inplace(cplx *const arr, const uint len) {
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
    butterfly_inplace(arr, len);
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

void fft_prettify(cplx *arr, const uint originallen, const uint upto) {
    const uint bound = uint_min(originallen / 2, upto);
    const float normalize = 2.f / (float)(originallen);

    for (uint i = 0; i < bound; i++) {
        arr[i] *= (log2f((float)(i + 1)) * normalize);
    }
}
