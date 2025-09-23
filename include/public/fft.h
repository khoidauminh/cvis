#ifndef CVIS_FFT_H
#define CVIS_FFT_H

#include "common.h"

void fft_inplace(tCplx *arr, tUint len);
void fft_inplace_stereo(tCplx *arr, tUint len, tUint upto);
void fft_io_stereo(tCplx *in, tCplx *out, tUint len, tUint upto);
void fft_prettify(tCplx *arr, tUint originallen, tUint prettifylen);

#endif
