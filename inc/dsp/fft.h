#ifndef FFT_H
#define FFT_H

#include <vector>
#include <iostream>
#include <cmath>
#include <complex.h>
#include <fftw3.h>
#include "utils/macros.h"
#include "utils/types.h"

namespace dsp {
namespace fft {

int find_optimal_fft_size(int desiredSize);
cdouble_vec fft1d(const cdouble_vec& in, int sign);
cdouble_vec fft2d(const cdouble_vec& in, int sign, int nx, int ny);
cdouble_vec fft3d(const cdouble_vec& in, int sign, int nx, int ny, int nz);

class FFT_Base {
public:
    FFT_Base();
    FFT_Base(int size, int rank);
    virtual ~FFT_Base();

    FFT_Base(FFT_Base&& other) noexcept;
    FFT_Base& operator=(FFT_Base&& other) noexcept;

    FFT_Base(const FFT_Base&) = delete;
    FFT_Base& operator=(const FFT_Base&) = delete;

    void execute(const cdouble_vec& in, cdouble_vec& out, int sign);

protected:
    int size_;
    fftw_plan plan_forward_;
    fftw_plan plan_backward_;
    fftw_complex* in_;
    fftw_complex* out_;
};

class FFT1D : public FFT_Base {
public:
    FFT1D() : FFT_Base() {}
    FFT1D(int size);
    ~FFT1D();

    FFT1D(FFT1D&& other) noexcept;
    FFT1D& operator=(FFT1D&& other) noexcept;
};

class FFT2D : public FFT_Base {
public:
    FFT2D() : FFT_Base(), nx_(0), ny_(0) {}
    FFT2D(int nx, int ny);
    ~FFT2D();

    FFT2D(FFT2D&& other) noexcept;
    FFT2D& operator=(FFT2D&& other) noexcept;

private:
    int nx_, ny_;
};

class FFT3D : public FFT_Base {
public:
    FFT3D() : FFT_Base(), nx_(0), ny_(0), nz_(0) {}
    FFT3D(int nx, int ny, int nz);
    ~FFT3D();

    FFT3D(FFT3D&& other) noexcept;
    FFT3D& operator=(FFT3D&& other) noexcept;

private:
    int nx_, ny_, nz_;
};
    

} // namespace fft
} // namespace dsp

#endif // FFT_H
