#include <SDL3/SDL_atomic.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "logging.h"

static SDL_AtomicInt quiet = {0};

void set_logging_disabled(bool b) { SDL_SetAtomicInt(&quiet, (int)b); }

bool is_logging_disabled() { return SDL_GetAtomicInt(&quiet) == 0; }

#define PERFORM_PRINT                                                          \
    va_list argptr;                                                            \
    va_start(argptr, msg);                                                     \
    vfprintf(stderr, msg, argptr);                                             \
    va_end(argptr);

void info(const char *msg, ...) {
    if (is_logging_disabled())
        return;

    PERFORM_PRINT
}

void warn(const char *msg, ...) {
    if (is_logging_disabled())
        return;

    printf("WARNING: ");

    PERFORM_PRINT;
}

void die(const char *msg, ...) {
    if (is_logging_disabled())
        return;

    PERFORM_PRINT;

    abort();
}
