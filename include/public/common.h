#ifndef CVIS_COMMON_H
#define CVIS_COMMON_H

#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_stdinc.h>
#include <complex.h>

typedef float complex tCplx;
typedef unsigned long long tInstant;
typedef unsigned char tUbyte;
typedef unsigned long tUlong;
typedef unsigned int tUint;

typedef SDL_Color sColor;
typedef SDL_BlendMode eBlendMode;

typedef struct int_2d {
    int x;
    int y;
} sInt2d;

typedef struct uint_2d {
    tUint x;
    tUint y;
} sUint2d;

typedef enum text_alignment {
    CVIS_TEXTALIGN_LEFT,
    CVIS_TEXTALIGN_MIDDLE,
    CVIS_TEXTALIGN_RIGHT,
} eTextAlignment;

typedef enum text_anchor {
    CVIS_TEXTANCHOR_BOTTOM,
    CVIS_TEXTANCHOR_MIDDLE,
    CVIS_TEXTANCHOR_TOP,
} eTextAnchor;

constexpr float PI = 3.14159265359f;
constexpr float TAU = PI * 2.0f;
constexpr tInstant INSTANT_SECOND = 1000;
constexpr tUlong ONEBILLION = 1'000'000'000;

tUint uint_min(tUint a, tUint b);
tUint uint_max(tUint a, tUint b);
int int_max(int a, int b);

tUint ulog2(tUint x);

tCplx quad1(tCplx x);
float l1norm(tCplx x);
float clampf(float, float, float);
float cmaxf(tCplx x);

tInstant instant();

#endif
