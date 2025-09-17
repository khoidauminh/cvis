#include <SDL3/SDL_atomic.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "public/logging.h"

static SDL_AtomicInt quiet = {0};

void set_logging_disabled(bool b) { SDL_SetAtomicInt(&quiet, (int)b); }

bool is_logging_disabled() { return SDL_GetAtomicInt(&quiet) == 1; }

#define PERFORM_PRINT(io)                                                      \
    va_list argptr;                                                            \
    va_start(argptr, msg);                                                     \
    vfprintf(io, msg, argptr);                                                 \
    va_end(argptr);

void info(const char *msg, ...) {
    if (is_logging_disabled())
        return;

    PERFORM_PRINT(stdout)
}

void warn(const char *msg, ...) {
    if (is_logging_disabled())
        return;

    fprintf(stderr, "\x1B[95;1m");
    fprintf(stderr, "!!! WARNING: ");

    PERFORM_PRINT(stderr);

    fprintf(stderr, "\x1B[0m");
}

void error(const char *msg, ...) {
    fprintf(stderr, "\x1B[31;1m");
    fprintf(stderr, "!!! ERROR: ");

    PERFORM_PRINT(stderr);

    fprintf(stderr, "\x1B[0m");
}

void die(const char *msg, ...) {
    fprintf(stderr, "\x1B[31;1m");
    fprintf(stderr, "!!! ERROR: ");

    PERFORM_PRINT(stderr);

    fprintf(stderr, "\x1B[0m");

    abort();
}
