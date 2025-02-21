#ifndef FFT_H
#define FFT_H

#include <vector>
#include <iostream>
#include <cmath>
#include <complex>
#include <fftw3.h>
#include "utils/types.h"

namespace dsp {
namespace fft {

int find_optimal_fft_size(int desiredSize);
void fft1d(const cdouble_vec& in, cdouble_vec& out, int sign);
void fft2d(const cdouble_vec& in, cdouble_vec& out, int sign, int nx, int ny);
void fft3d(const cdouble_vec& in, cdouble_vec& out, int sign, int nx, int ny, int nz);

class FFT1D {
public:
    FFT1D(int size);
    ~FFT1D();
    void execute(const cdouble_vec& in, cdouble_vec& out, int sign);

private:
    int size_;
    fftw_plan plan_forward_;
    fftw_plan plan_backward_;
};

class FFT2D {
public:
    FFT2D(int nx, int ny);
    ~FFT2D();
    void execute(const cdouble_vec& in, cdouble_vec& out, int sign);

private:
    int nx_, ny_;
    fftw_plan plan_forward_;
    fftw_plan plan_backward_;
};

class FFT3D {
public:
    FFT3D(int nx, int ny, int nz);
    ~FFT3D();
    void execute(const cdouble_vec& in, cdouble_vec& out, int sign);

private:
    int nx_, ny_, nz_;
    fftw_plan plan_forward_;
    fftw_plan plan_backward_;
};

} // namespace fft
} // namespace dsp

#endif // FFT_H