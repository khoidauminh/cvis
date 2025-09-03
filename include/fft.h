#ifndef CVIS_FFT_H
#define CVIS_FFT_H

#include "common.h"

uint ulog2(uint x);
void fft_inplace(cplx *arr, uint len);
void fft_inplace_stereo(cplx *arr, uint len, uint upto);
void fft_io_stereo(cplx *in, cplx *out, uint len, uint upto);
void fft_prettify(cplx *arr, uint originallen, uint prettifylen);

#endif
