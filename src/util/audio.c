#include <miniaudio.h>

#include "declare.h"
#include "logging.h"
#include <stdlib.h>

const uint POWER = 12;
const uint BUFFER_SIZE = 1 << POWER;
const uint BUFFER_MASK = BUFFER_SIZE - 1;
const uint SAMPLERATE = 44100;

typedef struct audiobuffer {
    uint offset;
    uint write;
    cplx max;

    uint rotates_since_last_write;
    uint auto_rotate_size;
    uint samples_rotated;

    cplx data[];
} AudioBuffer;

static AudioBuffer *gbuffer = NULL;
static ma_device gdevice;

void data_callback(ma_device *, void *restrict, const void *restrict pInput,
                   uint frame_count) {
    const cplx *buffer =
        pInput; // direct cast allowed since we're forcing f32 format.

    int input_size = frame_count / 2;
    int amount_left = input_size;

    int write = gbuffer->write;

    while (amount_left > 0) {
        int avail_size = BUFFER_SIZE - write;

        uint write_amount =
            (avail_size < amount_left) ? avail_size : amount_left;

        memcpy(gbuffer->data + write, buffer, sizeof(cplx[write_amount]));

        amount_left -= write_amount;
        write += write_amount;
        write &= BUFFER_MASK;
    }

    gbuffer->offset =
        (input_size >= BUFFER_SIZE)
            ? write // Last write was overwritten so start at new write.
            : (gbuffer->write - gbuffer->samples_rotated) &
                  BUFFER_MASK; // estimates the amount of samples will be read
                               // based on the last write.

    gbuffer->write = write;

    gbuffer->auto_rotate_size =
        input_size / (gbuffer->rotates_since_last_write + 1) + 1;

    gbuffer->rotates_since_last_write = 0;
    gbuffer->samples_rotated = 0;
}

cplx *buffer_get(uint index) {
    return gbuffer->data + ((index + gbuffer->offset) & BUFFER_MASK);
}

void buffer_read(cplx *restrict cplx_array, uint amount) {
    uint index = 0;
    uint offset = gbuffer->offset;

    amount = SDL_min(amount, BUFFER_SIZE);

    while (amount > 0) {
        int avail_size = BUFFER_SIZE - offset;

        uint write_amount = (avail_size < amount) ? avail_size : amount;

        memcpy(cplx_array + index, gbuffer->data + offset,
               sizeof(cplx[write_amount]));

        index += write_amount;
        amount -= write_amount;
        offset += write_amount;
        offset &= BUFFER_MASK;
    }
}

void buffer_rotate_left(uint ammount) {
    gbuffer->offset += ammount;
    gbuffer->offset &= BUFFER_MASK;
    gbuffer->samples_rotated += ammount;
}

void buffer_auto_rotate() {
    buffer_rotate_left(gbuffer->auto_rotate_size);
    gbuffer->rotates_since_last_write += 1;
}

void init_audio() {
    gbuffer = calloc(1, sizeof(AudioBuffer) + sizeof(cplx[BUFFER_SIZE]));

    if (gbuffer == NULL) {
        die("Failed to allocate audio buffer.");
    }

    gbuffer->max = 0.0f;
    gbuffer->offset = 0;
    gbuffer->write = 0;

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
