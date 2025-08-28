#include <miniaudio.h>

#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "audio.h"
#include "declare.h"
#include "interpolation.h"

constexpr uint CHUNK_SIZE = SAMPLERATE * 30 / 1000;
constexpr uint BUFFER_SIZE = 1 << 15;
constexpr uint BUFFER_MASK = BUFFER_SIZE - 1;
constexpr uint DEFAULT_ROTATE_SIZE = CHUNK_SIZE / 4;
constexpr float NORMALIZE_SPEED_FACTOR = 0.97f;
constexpr float NORMALIZE_MIN_THRESHOLD = 0.01f;
constexpr float NORMALIZE_MAX_THRESHOLD = 1.0f;

typedef struct audiobuffer {
    uint start;

    uint write;
    uint oldwrite;
    uint lastwritesize;

    uint autorotatesize;
    uint rotatessinceupdate;

    float max;

    cplx data[BUFFER_SIZE];
} AudioBuffer;

static AudioBuffer *gbuffer = nullptr;
static ma_device gdevice;

void buffer_normalize();

void data_callback(ma_device *, void *restrict, const void *restrict pInput,
                   unsigned int frame_count) {
    const cplx *buffer =
        pInput; // direct cast allowed since we're forcing f32 format.

    const uint input_size = frame_count / 2;
    uint amount_left = input_size;

    gbuffer->oldwrite = gbuffer->write;

    while (amount_left > 0) {
        uint available = BUFFER_SIZE - gbuffer->write;

        uint write_amount = (amount_left < available) ? amount_left : available;

        memcpy(gbuffer->data + gbuffer->write, buffer,
               sizeof(cplx) * write_amount);

        buffer += write_amount;
        amount_left -= write_amount;
        gbuffer->write += write_amount;
        gbuffer->write &= BUFFER_MASK;
    }

    gbuffer->lastwritesize = input_size;

    gbuffer->start = (gbuffer->write - CHUNK_SIZE) & BUFFER_MASK;

    gbuffer->autorotatesize = input_size / (gbuffer->rotatessinceupdate + 3);

    gbuffer->rotatessinceupdate = 0;

    buffer_normalize();
}

void buffer_normalize() {
    const uint bound = gbuffer->lastwritesize;

    float max = 0.0f;

    for (uint i = 0; i < bound; i++) {
        uint index = (i + gbuffer->oldwrite) & BUFFER_MASK;
        max = fmaxf(cmaxf(gbuffer->data[index]), max);
    }

    // Buffer is empty so skip normalization.
    if (max < NORMALIZE_MIN_THRESHOLD) {
        return;
    }

    max = clampf(NORMALIZE_MIN_THRESHOLD, max, NORMALIZE_MAX_THRESHOLD);

    gbuffer->max = decay(gbuffer->max, max, NORMALIZE_SPEED_FACTOR);

    float scale = 1.0 / gbuffer->max;

    for (uint i = 0; i < bound; i++) {
        uint index = (i + gbuffer->oldwrite) & BUFFER_MASK;
        gbuffer->data[index] *= scale;
    }
}

cplx *buffer_get(uint index) {
    return gbuffer->data + (index + gbuffer->start);
}

uint buffer_read(cplx *cplx_array, uint amount) {
    amount = amount < CHUNK_SIZE ? amount : CHUNK_SIZE;
    uint return_amount = amount;

    uint start = gbuffer->start;

    while (amount > 0) {
        uint available = BUFFER_SIZE - start;
        uint write_amount = (amount < available) ? amount : available;

        memcpy(cplx_array, gbuffer->data + start, sizeof(cplx) * write_amount);

        amount -= write_amount;
        cplx_array += write_amount;
        start += write_amount;
        start &= BUFFER_MASK;
    }

    return return_amount;
}

void buffer_autoslide() {
    gbuffer->rotatessinceupdate += 1;
    gbuffer->start += gbuffer->autorotatesize;
    gbuffer->start &= BUFFER_MASK;
}

void init_audio() {
    assert(!gbuffer); // Don't allow 2nd innitialization.

    gbuffer = calloc(1, sizeof(AudioBuffer));
    assert(gbuffer);

    gbuffer->autorotatesize = DEFAULT_ROTATE_SIZE;

    ma_result result;
    ma_device_config deviceConfig;

    deviceConfig = ma_device_config_init(ma_device_type_capture);
    deviceConfig.capture.format = ma_format_f32;
    deviceConfig.capture.channels = 2;
    deviceConfig.sampleRate = SAMPLERATE;
    deviceConfig.dataCallback = data_callback;

    result = ma_device_init(nullptr, &deviceConfig, &gdevice);

    assert(result == MA_SUCCESS);

    result = ma_device_start(&gdevice);
    assert(result == MA_SUCCESS);
}

void free_audio() {
    ma_device_uninit(&gdevice);
    free(gbuffer);

    memset(&gdevice, 0, sizeof(gdevice));
    gbuffer = nullptr;
}

void normalize_average(cplx *samples, uint len) {
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
    static float max = 0.0f;

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
    float average;
} MovingAverage;

MovingAverage moving_average_new(float *buffer, uint size, float val) {
    MovingAverage ma;
    ma.size = size;
    ma.index = 0;
    ma.sum = (float)size * val;
    ma.average = val;
    ma.denom = 1.0 / (float)size;
    ma.data = buffer;

    for (uint i = 0; i < size; i++)
        ma.data[i] = val;

    return ma;
}

float moving_average_pop(MovingAverage *ma, float val) {
    float out = ma->data[ma->index];

    ma->data[ma->index] = val;

    ma->index += 1;
    ma->index %= ma->size;

    return out;
}

float moving_average_update(MovingAverage *ma, float val) {
    float old = moving_average_pop(ma, val);

    ma->sum = ma->sum - old + val;

    ma->average = ma->denom * ma->sum;

    return ma->average;
}

typedef struct numpair {
    uint index;
    float val;
} Numpair;

typedef struct moving_maximum {
    Numpair *data;
    uint len;
    uint index;
    uint size;
} MovingMaximum;

MovingMaximum moving_maximum_new(Numpair *buffer, uint size) {
    return (MovingMaximum){
        .data = buffer,
        .len = 0,
        .index = 0,
        .size = size,
    };
}

void moving_maximum_push(MovingMaximum *mm, Numpair new) {
    uint i = mm->len;

    mm->len += 1;

    while (i > 0) {
        uint p = (i - 1) / 2;

        if (mm->data[p].val >= new.val) {
            break;
        }

        mm->data[i] = mm->data[p];

        i = p;
    }

    mm->data[i] = new;
}

Numpair *moving_maximum_peek(MovingMaximum *mm) { return mm->data; }

Numpair moving_maximum_pop(MovingMaximum *mm, uint p) {
    mm->len -= 1;
    Numpair out = mm->data[0];

    mm->data[0] = mm->data[mm->len];

    const uint bound = mm->len - 2;
    uint i = 2 * p + 1;

    while (i < bound) {
        i += (mm->data[i].val <= mm->data[i + 1].val);

        if (mm->data[p].val >= mm->data[i].val) {
            return out;
        }

        mm->data[p] = mm->data[i];

        p = i;
        i = i * 2 + 1;
    }

    if (i == mm->len - 1 && mm->data[p].val < mm->data[i].val) {
        mm->data[p] = mm->data[i];
    }

    return out;
}

float moving_maximum_update(MovingMaximum *mm, float new) {
    moving_maximum_push(mm, (Numpair){.index = mm->index, .val = new});

    uint max_age = moving_maximum_peek(mm)->index + mm->size - 1;
    // printf("%lu %lu\n", max_age, mm->index);

    if (max_age <= mm->index) {
        moving_maximum_pop(mm, 0);
    }

    mm->index += 1;

    return moving_maximum_peek(mm)->val;
}

constexpr int MMAX_WINDOW_SIZE = 20;
constexpr int MAVE_WINDOW_SIZE = MMAX_WINDOW_SIZE * 3 / 4;
constexpr int MMAX_CAPACITY = 256;

static float mave_buffer[MAVE_WINDOW_SIZE];
static Numpair mmax_buffer[MMAX_CAPACITY];

void compress(cplx *samples, uint len, float limit, float gain) {
    assert(len + MMAX_WINDOW_SIZE <= MMAX_CAPACITY);

    MovingAverage mave =
        moving_average_new(mave_buffer, MAVE_WINDOW_SIZE, limit);
    MovingMaximum mmax = moving_maximum_new(mmax_buffer, MMAX_WINDOW_SIZE);

    const int length = (int)len;
    const int bound = (int)len + MAVE_WINDOW_SIZE;

    for (int i = 0; i < bound; i++) {
        float smp = (i < length) ? fmaxf(cmaxf(samples[i]), limit) : limit;

        float mult =
            moving_average_update(&mave, moving_maximum_update(&mmax, smp));

        int j = i - MAVE_WINDOW_SIZE;

        if (j >= 0 && j < bound) {
            float scale = gain / mult;
            samples[j] *= scale;
        }
    }
}
