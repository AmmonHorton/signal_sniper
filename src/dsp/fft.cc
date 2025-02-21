#include "dsp/fft.h"

namespace dsp {
namespace fft {

int find_optimal_fft_size(int desiredSize) {
    // Define the search range: 0.1x to 10x of the desired size
    int minSize = std::max(1, desiredSize - desiredSize / 10);
    int maxSize = desiredSize + desiredSize / 10;

    // Generate a list of power-of-two FFT sizes within the range
    std::vector<int> candidateSizes;
    for (int size = 1; size <= maxSize; size *= 2) {
        if (size >= minSize) {
            candidateSizes.push_back(size);
        }
    }

    // Find the fastest FFT size
    int bestSize = candidateSizes[0];
    double bestCost = std::numeric_limits<double>::max();

    for (int size : candidateSizes) {
        fftw_plan plan = fftw_plan_dft_1d(size, nullptr, nullptr, FFTW_FORWARD, FFTW_ESTIMATE);
        double cost = fftw_estimate_cost(plan);
        fftw_destroy_plan(plan);

        if (cost < bestCost) {
            bestSize = size;
            bestCost = cost;
        }
    }

    return bestSize;
}

void fft1d(const std::vector<std::complex<double>>& in, std::vector<std::complex<double>>& out, int sign) {
    int size = in.size();
    fftw_plan plan = fftw_plan_dft_1d(size, 
                                      reinterpret_cast<fftw_complex*>(const_cast<std::complex<double>*>(in.data())), 
                                      reinterpret_cast<fftw_complex*>(out.data()), 
                                      sign, 
                                      FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
}

void fft2d(const std::vector<std::complex<double>>& in, std::vector<std::complex<double>>& out, int sign, int nx, int ny) {
    fftw_plan plan = fftw_plan_dft_2d(nx, ny, 
                                      reinterpret_cast<fftw_complex*>(const_cast<std::complex<double>*>(in.data())), 
                                      reinterpret_cast<fftw_complex*>(out.data()), 
                                      sign, 
                                      FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
}

void fft3d(const std::vector<std::complex<double>>& in, std::vector<std::complex<double>>& out, int sign, int nx, int ny, int nz) {
    fftw_plan plan = fftw_plan_dft_3d(nx, ny, nz, 
                                      reinterpret_cast<fftw_complex*>(const_cast<std::complex<double>*>(in.data())), 
                                      reinterpret_cast<fftw_complex*>(out.data()), 
                                      sign, 
                                      FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
}

// FFT1D Implementation
FFT1D::FFT1D(int size) : size_(size) {
    plan_forward_ = fftw_plan_dft_1d(size_, nullptr, nullptr, FFTW_FORWARD, FFTW_ESTIMATE);
    plan_backward_ = fftw_plan_dft_1d(size_, nullptr, nullptr, FFTW_BACKWARD, FFTW_ESTIMATE);
}

FFT1D::~FFT1D() {
    fftw_destroy_plan(plan_forward_);
    fftw_destroy_plan(plan_backward_);
}

void FFT1D::execute(const std::vector<std::complex<double>>& in, std::vector<std::complex<double>>& out, int sign) {
    if (sign == FFTW_FORWARD) {
        fftw_execute_dft(plan_forward_, reinterpret_cast<fftw_complex*>(const_cast<std::complex<double>*>(in.data())), reinterpret_cast<fftw_complex*>(out.data()));
    } else {
        fftw_execute_dft(plan_backward_, reinterpret_cast<fftw_complex*>(const_cast<std::complex<double>*>(in.data())), reinterpret_cast<fftw_complex*>(out.data()));
    }
}

// FFT2D Implementation
FFT2D::FFT2D(int nx, int ny) : nx_(nx), ny_(ny) {
    plan_forward_ = fftw_plan_dft_2d(nx_, ny_, nullptr, nullptr, FFTW_FORWARD, FFTW_ESTIMATE);
    plan_backward_ = fftw_plan_dft_2d(nx_, ny_, nullptr, nullptr, FFTW_BACKWARD, FFTW_ESTIMATE);
}

FFT2D::~FFT2D() {
    fftw_destroy_plan(plan_forward_);
    fftw_destroy_plan(plan_backward_);
}

void FFT2D::execute(const std::vector<std::complex<double>>& in, std::vector<std::complex<double>>& out, int sign) {
    if (sign == FFTW_FORWARD) {
        fftw_execute_dft(plan_forward_, reinterpret_cast<fftw_complex*>(const_cast<std::complex<double>*>(in.data())), reinterpret_cast<fftw_complex*>(out.data()));
    } else {
        fftw_execute_dft(plan_backward_, reinterpret_cast<fftw_complex*>(const_cast<std::complex<double>*>(in.data())), reinterpret_cast<fftw_complex*>(out.data()));
    }
}

// FFT3D Implementation
FFT3D::FFT3D(int nx, int ny, int nz) : nx_(nx), ny_(ny), nz_(nz) {
    plan_forward_ = fftw_plan_dft_3d(nx_, ny_, nz_, nullptr, nullptr, FFTW_FORWARD, FFTW_ESTIMATE);
    plan_backward_ = fftw_plan_dft_3d(nx_, ny_, nz_, nullptr, nullptr, FFTW_BACKWARD, FFTW_ESTIMATE);
}

FFT3D::~FFT3D() {
    fftw_destroy_plan(plan_forward_);
    fftw_destroy_plan(plan_backward_);
}

void FFT3D::execute(const std::vector<std::complex<double>>& in, std::vector<std::complex<double>>& out, int sign) {
    if (sign == FFTW_FORWARD) {
        fftw_execute_dft(plan_forward_, reinterpret_cast<fftw_complex*>(const_cast<std::complex<double>*>(in.data())), reinterpret_cast<fftw_complex*>(out.data()));
    } else {
        fftw_execute_dft(plan_backward_, reinterpret_cast<fftw_complex*>(const_cast<std::complex<double>*>(in.data())), reinterpret_cast<fftw_complex*>(out.data()));
    }
}

} // namespace fft
} // namespace dsp