#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "declare.h"
#include "logging.h"
#include "vec.h"

const uint INIT_CAPACITY = 16;

typedef struct vector vec;

vec vec_new(uint itemsize) { return (vec){.itemsize = itemsize}; }
vec string_new() { return (vec){.itemsize = sizeof(char)}; }

void *vec_get(vec *v, uint index) {
#ifdef BOUNDS_CHECK
    if (index >= v->length)
        die("Index out of bounds.");
#endif

    return v->data + v->itemsize * index;
}

void vec_init(vec *v, uint cap) {
    if (v->data) {
        die("Vector is already initialized.");
    }

    if ((v->data = malloc(cap * v->itemsize)) == NULL) {
        die("Failed to initialize the vector.");
    }

    v->length = 0;
    v->capacity = cap;
}

void vec_grow(vec *v) {
    if (v->capacity == 0) {
        vec_init(v, INIT_CAPACITY);
        return;
    }

    uint newcapacity = v->capacity * 2;
    void *newdata = realloc(v->data, v->itemsize * newcapacity);

    if (newdata == NULL) {
        die("Failed to grow the vector.");
    }

    v->data = newdata;
    v->capacity = newcapacity;
}

void vec_push(vec *v, void *item) {
    if (v->length == v->capacity) {
        vec_grow(v);
    }

    memcpy(v->data + v->length * v->itemsize, item, v->itemsize);

    v->length += 1;
}

void *vec_pop(vec *v) {
    if (v->length == 0) {
        die("Vector is empty.");
    }

    v->length -= 1;

    void *out = v->data + v->length * v->itemsize;

    return out;
}

void vec_clear(vec *v) { v->length = 0; }

uint vec_len(const vec *const v) { return v->length; }

void vec_free(Vector *v) {
    free(v->data);
    memset(v, 0, sizeof(*v));
}

String string_with_capacity(uint cap) {
    auto str = string_new();
    vec_init(&str, cap);
    return str;
}

void string_cat(String *str, const char *tail) {
    uint len = strlen(tail);
    uint finallength = str->length + len;

    while (finallength >= str->capacity) {
        vec_grow(str);
    }

    memcpy(str->data + str->length, tail, len);

    str->length = finallength;
}

void string_reserve_at_least(String *str, uint amount) {
    if (str->length + amount < str->capacity) {
        return;
    }

    uint newcapacity = str->capacity + amount;

    void *newdata = realloc(str->data, newcapacity);
    assert(newdata != NULL);

    str->data = newdata;
    str->capacity = newcapacity;
}

void string_clear(String *str) { str->length = 0; }

char *string_data(String *str) { return str->data; }
