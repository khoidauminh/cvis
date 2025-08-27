#include <miniaudio.h>

#include "declare.h"
#include "logging.h"
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>

constexpr uint SAMPLERATE = 44100;
constexpr uint CHUNK_SIZE = SAMPLERATE * 30 / 1000;
constexpr uint BUFFER_SIZE = 1 << 15;
constexpr uint BUFFER_MASK = BUFFER_SIZE - 1;

typedef struct audiobuffer {
    uint start;

    uint write;
    uint idealstart;
    uint rotated;

    cplx data[BUFFER_SIZE];
} AudioBuffer;

static AudioBuffer *gbuffer = NULL;
static ma_device gdevice;

void data_callback(ma_device *, void *restrict, const void *restrict pInput,
                   uint frame_count) {
    const cplx *buffer =
        pInput; // direct cast allowed since we're forcing f32 format.

    const int input_size = frame_count / 2;
    int amount_left = input_size;

    while (amount_left > 0) {
        uint available = BUFFER_SIZE - gbuffer->write;

        uint write_amount = (amount_left < available) ? amount_left : available;

        memcpy(gbuffer->data + gbuffer->write, buffer,
               sizeof(cplx[write_amount]));

        buffer += write_amount;
        amount_left -= write_amount;
        gbuffer->write += write_amount;
        gbuffer->write &= BUFFER_MASK;
    }

    gbuffer->idealstart = (gbuffer->write - CHUNK_SIZE) & BUFFER_MASK;
    gbuffer->start = gbuffer->idealstart;
    gbuffer->rotated = 0;
}

cplx *buffer_get(uint index) {
    return gbuffer->data + (index + gbuffer->start);
}

uint buffer_read(cplx *restrict cplx_array, uint amount) {
    amount = SDL_min(amount, CHUNK_SIZE);
    uint return_amount = amount;

    uint start = gbuffer->start;

    while (amount > 0) {
        int available = BUFFER_SIZE - start;
        uint write_amount = (amount < available) ? amount : available;

        memcpy(cplx_array, gbuffer->data + start, sizeof(cplx[write_amount]));

        amount -= write_amount;
        cplx_array += write_amount;
        start += write_amount;
        start &= BUFFER_MASK;
    }

    return return_amount;
}

void buffer_slide(uint ammount) {
    if ((gbuffer->rotated += ammount) >= CHUNK_SIZE) {
        gbuffer->start = gbuffer->idealstart;
        return;
    }

    gbuffer->start += ammount;
    gbuffer->start &= BUFFER_MASK;
}

void init_audio() {
    assert(!gbuffer); // Don't allow 2nd innitialization.

    gbuffer = calloc(1, sizeof(AudioBuffer));

    if (gbuffer == NULL) {
        die("Failed to allocate audio buffer.");
    }

    ma_result result;
    ma_device_config deviceConfig;

    deviceConfig = ma_device_config_init(ma_device_type_capture);
    deviceConfig.capture.format = ma_format_f32;
    deviceConfig.capture.channels = 2;
    deviceConfig.sampleRate = SAMPLERATE;
    deviceConfig.dataCallback = data_callback;

    result = ma_device_init(NULL, &deviceConfig, &gdevice);

    if (result != MA_SUCCESS) {
        die("Failed to initialize capture device.\n");
    }

    result = ma_device_start(&gdevice);
    if (result != MA_SUCCESS) {
        die("Failed to start device.\n");
    }
}

void free_audio() {
    ma_device_uninit(&gdevice);
    free(gbuffer);
}

// constexpr uint MOVING_AVERAGE_SIZE = 16;

// typedef struct movingaverage {
//     float window[MOVING_AVERAGE_SIZE];
//     float average;
//     uint index;
// } MovingAverage;

// void moving_average_update()

void normalize_average(cplx *samples, uint len) {
    float sum_of_squares = 0.0;
    for (uint i = 0; i < len; i++) {
        float sl = crealf(samples[i]);
        float sr = cimagf(samples[i]);

        sum_of_squares += sl * sl + sr * sr;
    }

    float average = clampf(10.f, sqrtf(sum_of_squares) / len, 100.f);

    float scale = 1.0 / average;

    for (uint i = 0; i < len; i++) {
        samples[i] *= scale;
    }
}

void normalize_max(cplx *samples, uint len) {
    float max = 0.0f;

    for (uint i = 0; i < len; i++) {
        max = fmaxf(max, cmaxf(samples[i]));
    }

    float scale = 1.0 / clampf(0.5f, max, 100.f);

    for (uint i = 0; i < len; i++) {
        samples[i] *= scale;
    }
}
