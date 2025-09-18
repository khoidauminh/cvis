#include <miniaudio.h>

#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdarg.h>
#include <stdbit.h>
#include <stdlib.h>
#include <string.h>

#include "audio.h"
#include "public/common.h"
#include "public/interpolation.h"
#include "public/logging.h"

constexpr uint CHUNK_SIZE = SAMPLERATE * 30 / 1000;
constexpr uint CHANNELS = 2;
constexpr uint BUFFER_SIZE = 1 << 18;
constexpr uint BUFFER_MASK = BUFFER_SIZE - 1;
constexpr uint DEFAULT_ROTATE_SIZE = CHUNK_SIZE / 4;
constexpr float NORMALIZE_SPEED_FACTOR = 0.99f;
constexpr float NORMALIZE_MIN_THRESHOLD = 0.001f;

constexpr float ZEROF = 0.0f;
constexpr float ONEF = 1.0;

typedef struct audiobuffer {
    uint writeend;
    uint oldwriteend;

    uint readend;

    uint samplesscanned;
    uint autorotatesize;
    uint rotatessinceupdate;
    uint lastwritesize;

    ulong age;

    float max;

    cplx data[BUFFER_SIZE];
} AudioBuffer;

static AudioBuffer *gbuffer = nullptr;
static ma_device gdevice = {};
static ma_mutex locker = {};

static void buffer_normalize();

void cplxcpy(cplx *restrict dst, const cplx *restrict src, uint amount) {
    memcpy(dst, src, sizeof(cplx) * amount);
}

void cplxzero(cplx *restrict buf, uint amount) {
    memset(buf, 0, sizeof(cplx) * amount);
}

static void data_callback(ma_device *, void *restrict,
                          const void *restrict pInput,
                          const unsigned int input_size) {
    const cplx *buffer =
        pInput; // direct cast allowed since we're forcing f32 format.

    uint amount_left = input_size;

    ma_mutex_lock(&locker);

    gbuffer->oldwriteend = gbuffer->writeend;

    while (amount_left > 0) {
        uint available = BUFFER_SIZE - gbuffer->writeend;

        uint write_amount = (amount_left < available) ? amount_left : available;

        cplxcpy(gbuffer->data + gbuffer->writeend, buffer, write_amount);

        buffer += write_amount;
        amount_left -= write_amount;
        gbuffer->writeend += write_amount;
        gbuffer->writeend &= BUFFER_MASK;
    }

    gbuffer->lastwritesize = input_size;

    gbuffer->readend = (gbuffer->writeend - input_size) & BUFFER_MASK;

    gbuffer->autorotatesize =
        input_size >> (1 + stdc_bit_width(gbuffer->rotatessinceupdate));

    gbuffer->rotatessinceupdate = 0;

    gbuffer->age += 1;

    buffer_normalize();
    ma_mutex_unlock(&locker);
}

static void buffer_normalize() {
    const uint bound = gbuffer->lastwritesize;

    float max = 0.0f;

    for (uint i = 0; i < bound; i++) {
        uint index = (i + gbuffer->oldwriteend) & BUFFER_MASK;
        max = fmaxf(cmaxf(gbuffer->data[index]), max);
    }

    gbuffer->max = decay(gbuffer->max, max, NORMALIZE_SPEED_FACTOR);

    // Buffer is empty so skip normalization.
    if (max < NORMALIZE_MIN_THRESHOLD) {
        return;
    }

    float scale = 1.0f / gbuffer->max;

    for (uint i = 0; i < bound; i++) {
        uint index = (i + gbuffer->oldwriteend) & BUFFER_MASK;
        gbuffer->data[index] *= scale;
    }
}

ulong BUFFER_AGE() { return gbuffer->age; }

uint BUFFER_INPUTSIZE() { return gbuffer->lastwritesize; }

// Gets the ith sample into the past
cplx BUFFER_GET(uint index) {
    const uint i = gbuffer->readend - index;
    return gbuffer->data[i & BUFFER_MASK];
}

uint BUFFER_READ(cplx cplx_array[], uint amount) {
    amount = uint_min(BUFFER_SIZE, amount);
    uint return_amount = amount;

    ma_mutex_lock(&locker);

    uint start = (gbuffer->readend + BUFFER_SIZE - amount) & BUFFER_MASK;

    while (amount > 0) {
        uint available = BUFFER_SIZE - start;
        uint write_amount = (amount < available) ? amount : available;

        cplxcpy(cplx_array, gbuffer->data + start, write_amount);

        amount -= write_amount;
        cplx_array += write_amount;
        start += write_amount;
        start &= BUFFER_MASK;
    }

    ma_mutex_unlock(&locker);

    return return_amount;
}

void BUFFER_SLIDE(const uint amount) {
    ma_mutex_lock(&locker);
    gbuffer->rotatessinceupdate += 1;
    gbuffer->readend += amount;
    gbuffer->readend &= BUFFER_MASK;
    ma_mutex_unlock(&locker);
}

void BUFFER_AUTOSLIDE() { BUFFER_SLIDE(gbuffer->autorotatesize); }

void init_audio() {
    assert(!gbuffer); // Don't allow 2nd innitialization.

    gbuffer = calloc(1, sizeof(AudioBuffer));
    assert(gbuffer);

    gbuffer->autorotatesize = DEFAULT_ROTATE_SIZE;

    ma_device_config deviceConfig;
    deviceConfig = ma_device_config_init(ma_device_type_capture);
    deviceConfig.capture.format = ma_format_f32;
    deviceConfig.capture.channels = CHANNELS;
    deviceConfig.sampleRate = SAMPLERATE;
    deviceConfig.dataCallback = data_callback;

    ma_result ma_result;

    ma_result = ma_mutex_init(&locker);
    if (ma_result != MA_SUCCESS) {
        warn("Failed to initialize mutex.");
    }

    ma_result = ma_device_init(nullptr, &deviceConfig, &gdevice);
    if (ma_result != MA_SUCCESS) {
        die("Failed to initialize device.");
    }

    ma_result = ma_device_start(&gdevice);
    if (ma_result != MA_SUCCESS) {
        die("Failed to start device.");
    }
}

void free_audio() {
    ma_device_uninit(&gdevice);
    free(gbuffer);

    ma_mutex_uninit(&locker);

    memset(&gdevice, 0, sizeof(gdevice));
    gbuffer = nullptr;
}

void normalize_average(cplx samples[], uint len) {
    float sum_of_squares = 0.0;
    for (uint i = 0; i < len; i++) {
        float sl = crealf(samples[i]);
        float sr = cimagf(samples[i]);

        sum_of_squares += sl * sl + sr * sr;
    }

    float average = clampf(10.f, sqrtf(sum_of_squares) / (float)len, 100.f);

    float scale = 1.0f / average;

    for (uint i = 0; i < len; i++) {
        samples[i] *= scale;
    }
}

void slow_regain(cplx *samples, uint len, float gain, float t) {
    static thread_local float max = 0.0f;

    float newmax = 0.0f;

    for (uint i = 0; i < len; i++) {
        newmax = fmaxf(newmax, cmaxf(samples[i]));
    }

    max = linear_decay(max, newmax, t);

    float scale = 1.0f * gain / fmaxf(0.5f, max);

    for (uint i = 0; i < len; i++) {
        samples[i] *= scale;
    }
}

void normalize_max(cplx *samples, uint len, float gain) {
    float max = 0.0f;

    for (uint i = 0; i < len; i++) {
        max = fmaxf(max, cmaxf(samples[i]));
    }

    float scale = 1.0f / fmaxf(0.5f, max) * gain;

    for (uint i = 0; i < len; i++) {
        samples[i] *= scale;
    }
}

typedef struct moving_average {
    uint size;
    uint index;
    float *data;
    float sum;
    float denom;
} MovingAverage;

static MovingAverage moving_average_new(float buffer[const], uint size) {
    MovingAverage ma = {
        .size = size,
        .index = 0,
        .sum = (float)size * 0.0f,
        .denom = 1.0f / (float)size,
        .data = buffer,
    };

    memset(buffer, 0, size * sizeof(float));

    return ma;
}

static float moving_average_update(MovingAverage *ma, float val) {
    ma->sum -= ma->data[ma->index];
    ma->sum += val;

    ma->data[ma->index] = val;

    ma->index += 1;
    if (ma->index == ma->size)
        ma->index = 0;

    return ma->denom * ma->sum;
}

typedef struct numpair {
    uint index;
    float val;
} Numpair;

typedef struct moving_maximum {
    Numpair *data;

    uint head;
    uint tail;
    uint len;

    uint index;
    uint wsize;
} MovingMaximum;

static MovingMaximum moving_maximum_new(Numpair buffer[const], uint wsize) {
    return (MovingMaximum){
        .data = buffer,
        .wsize = wsize,

        .head = 0,
        .tail = wsize - 1,
        .len = 0,

        .index = 0,
    };
}

static void moving_maximum_queue(MovingMaximum *mm, float new) {
    mm->len += 1;

    mm->tail += 1;
    if (mm->tail == mm->wsize)
        mm->tail = 0;

    mm->data[mm->tail] = (Numpair){mm->index, new};
}

static void moving_maximum_dequeue_head(MovingMaximum *mm) {
    mm->len -= 1;

    mm->head += 1;
    if (mm->head == mm->wsize)
        mm->head = 0;
}

static void moving_maximum_dequeue_tail(MovingMaximum *mm) {
    mm->len -= 1;

    if (mm->tail == 0)
        mm->tail = mm->wsize;

    mm->tail -= 1;
}

static float moving_maximum_update(MovingMaximum *mm, float new) {
    if (mm->len && mm->data[mm->head].index + mm->wsize <= mm->index) {
        moving_maximum_dequeue_head(mm);
    }

    while (mm->len && mm->data[mm->tail].val < new) {
        moving_maximum_dequeue_tail(mm);
    }

    assert(mm->len < mm->wsize);

    moving_maximum_queue(mm, new);

    mm->index += 1;

    return mm->data[mm->head].val;
}

void compress(cplx samples[const], uint len, float lo, float hi) {
    constexpr uint WINDOW_SIZE = 10;
    float mave_buffer[WINDOW_SIZE];
    Numpair mmax_buffer[WINDOW_SIZE];

    const uint bound = len + WINDOW_SIZE;
    const uint delay = WINDOW_SIZE - 1;

    MovingAverage mave = moving_average_new(mave_buffer, WINDOW_SIZE);
    MovingMaximum mmax = moving_maximum_new(mmax_buffer, WINDOW_SIZE);

    for (uint i = 0; i < bound; i++) {
        float smp1 = (i < len) ? cmaxf(samples[i]) : ZEROF;

        float smp2 =
            moving_average_update(&mave, moving_maximum_update(&mmax, smp1));

        float scale = (smp2 > hi)   ? hi / smp2
                      : (smp2 < lo) ? lo / fmaxf(smp2, NORMALIZE_MIN_THRESHOLD)
                                    : ONEF;

        uint j = i - delay;

        if (j < bound) {
            samples[j] *= scale;
        }
    }
}
