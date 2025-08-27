#ifndef VEC_H
#define VEC_H

#include "declare.h"

struct vector {
    void *data;
    uint length;
    uint capacity;
    uint itemsize;
};

typedef struct vector Vector;
typedef struct vector String;

Vector vec_new(uint itemsize);
void vec_push(Vector*, void*);
void *vec_pop(Vector*);
void vec_clear(Vector *v);
uint vec_len(const Vector *const v);
void vec_free(Vector*);
void *vec_get(Vector *v, uint index);

String string_with_capacity(uint cap);
void string_cat(String *str, const char *tail);
void string_clear(String *str);
void string_reserve_at_least(String *str, uint amount);
char *string_data(String *str);

#endif
