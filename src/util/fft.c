#include "public/fft.h"
#include "public/common.h"

#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdbit.h>
#include <stdlib.h>

constexpr tUint MAX_FFT_POWER = 13;
constexpr tUint MAX_FFT_LENGTH = 1 << MAX_FFT_POWER;

static tCplx *TWIDDLE_ARRAY = nullptr;
static tUint *BUTTERFLY_ARRAY = nullptr;

static void free_twiddle_array() {
    free(TWIDDLE_ARRAY);
    TWIDDLE_ARRAY = nullptr;
}

static void contruct_twiddle_array() {
    TWIDDLE_ARRAY = malloc(sizeof(tCplx) * (MAX_FFT_LENGTH + 1));
    assert(TWIDDLE_ARRAY);

    tUint i = 1;

    for (tUint k = 1; k < MAX_FFT_LENGTH; k *= 2) {
        float angle = -PI / (float)k;

        for (tUint j = 0; j < k; j++) {
            tCplx twiddle = cexpf((float)j * angle * I);

            TWIDDLE_ARRAY[i++] = twiddle;
        }
    }

    atexit(free_twiddle_array);
}

static tUint reverse_bit(tUint index, tUint power) {
    tUint out = 0;

    for (tUint i = 0; i < power; i++) {
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
    BUTTERFLY_ARRAY = malloc(sizeof(tUint) * MAX_FFT_LENGTH);
    assert(BUTTERFLY_ARRAY);

    for (tUint power = 0; power < MAX_FFT_POWER; power++) {
        tUint k = 1 << power;
        tUint *arr = BUTTERFLY_ARRAY + k;

        for (tUint j = 0; j < k; j++) {
            arr[j] = reverse_bit(j, power);
        }
    }

    atexit(free_butterfly_array);
}

static void butterfly_inplace(tCplx arr[const], tUint len) {
    if (BUTTERFLY_ARRAY == nullptr) {
        construct_butterfly_array();
    }

    const tUint *butterfy = BUTTERFLY_ARRAY + len;

    len -= 1;

    for (tUint i = 1; i < len; i++) {
        tUint ni = butterfy[i];
        if (ni > i) {
            tCplx z = arr[i];
            arr[i] = arr[ni];
            arr[ni] = z;
        }
    }
}

static void compute_fft_inplace(tCplx arr[const], const tUint len) {
    if (TWIDDLE_ARRAY == nullptr) {
        contruct_twiddle_array();
    }

    for (tUint half_window = 1; half_window < len; half_window *= 2) {
        const tUint window = half_window * 2;
        tCplx *root = TWIDDLE_ARRAY + half_window;

        for (tUint k = 0; k < len; k += window) {
            tCplx *slice_left = arr + k;
            tCplx *slice_right = arr + k + half_window;

            for (tUint j = 0; j < half_window; j++) {
                tCplx z = slice_right[j] * root[j];
                slice_right[j] = slice_left[j] - z;
                slice_left[j] += z;
            }
        }
    }
}

void fft_inplace(tCplx arr[const], tUint len) {
    butterfly_inplace(arr, len);
    compute_fft_inplace(arr, len);
}

void fft_inplace_stereo(tCplx arr[const], tUint len, tUint upto) {
    fft_inplace(arr, len);

    tUint bound = uint_min(len / 2, upto);

    for (tUint i = 1; i < bound; i++) {
        tCplx z1 = arr[i];
        tCplx z2 = conjf(arr[len - i]);
        arr[i] = CMPLXF(l1norm(z1 + z2), l1norm(z1 - z2));
    }
}

void fft_prettify(tCplx arr[const], const tUint originallen, const tUint upto) {
    const tUint bound = uint_min(originallen / 2, upto);
    const float normalize = 2.f / (float)(originallen);

    for (tUint i = 0; i < bound; i++) {
        arr[i] *= (log2f((float)(i + 1)) * normalize);
    }
}
