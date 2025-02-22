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

// 1D FFT using FFT1D class
cdouble_vec fft1d(const cdouble_vec& in, int sign) {
    int size = in.size();
    cdouble_vec out(size);
    
    FFT1D fft(size); // Use the FFT1D class
    fft.execute(in, out, sign);

    return out;
}

// 2D FFT using FFT2D class
cdouble_vec fft2d(const cdouble_vec& in, int sign, int nx, int ny) {
    int size = nx * ny;
    cdouble_vec out(size);

    FFT2D fft(nx, ny); // Use the FFT2D class
    fft.execute(in, out, sign);

    return out;
}

// 3D FFT using FFT3D class
cdouble_vec fft3d(const cdouble_vec& in, int sign, int nx, int ny, int nz) {
    int size = nx * ny * nz;
    cdouble_vec out(size);

    FFT3D fft(nx, ny, nz); // Use the FFT3D class
    fft.execute(in, out, sign);

    return out;
}


// FFT_Base Implementation
FFT_Base::FFT_Base() : size_(0), plan_forward_(nullptr), plan_backward_(nullptr), in_(nullptr), out_(nullptr) {}

FFT_Base::FFT_Base(int size, int rank) : size_(size) {
    in_ = fftw_alloc_complex(size_);
    out_ = fftw_alloc_complex(size_);
    
    plan_forward_ = fftw_plan_dft(rank, &size_, in_, out_, FFTW_FORWARD, FFTW_ESTIMATE);
    plan_backward_ = fftw_plan_dft(rank, &size_, in_, out_, FFTW_BACKWARD, FFTW_ESTIMATE);
}

FFT_Base::~FFT_Base() {
    if (plan_forward_) fftw_destroy_plan(plan_forward_);
    if (plan_backward_) fftw_destroy_plan(plan_backward_);
    if (in_) fftw_free(in_);
    if (out_) fftw_free(out_);
}

// Move constructor
FFT_Base::FFT_Base(FFT_Base&& other) noexcept 
    : size_(other.size_), plan_forward_(other.plan_forward_), plan_backward_(other.plan_backward_), 
      in_(other.in_), out_(other.out_) {
    other.plan_forward_ = nullptr;
    other.plan_backward_ = nullptr;
    other.in_ = nullptr;
    other.out_ = nullptr;
}

// Move assignment operator
FFT_Base& FFT_Base::operator=(FFT_Base&& other) noexcept {
    if (this != &other) {
        if (plan_forward_) fftw_destroy_plan(plan_forward_);
        if (plan_backward_) fftw_destroy_plan(plan_backward_);
        if (in_) fftw_free(in_);
        if (out_) fftw_free(out_);

        size_ = other.size_;
        plan_forward_ = other.plan_forward_;
        plan_backward_ = other.plan_backward_;
        in_ = other.in_;
        out_ = other.out_;

        other.plan_forward_ = nullptr;
        other.plan_backward_ = nullptr;
        other.in_ = nullptr;
        other.out_ = nullptr;
    }
    return *this;
}

void FFT_Base::execute(const cdouble_vec& in, cdouble_vec& out, int sign) {
    if (size_ == 0) return;

    // Copy input data to allocated FFTW buffer
    std::copy(in.begin(), in.begin() + size_, reinterpret_cast<std::complex<double>*>(in_));
    fftw_execute_dft((sign == FFTW_FORWARD ? plan_forward_ : plan_backward_),
                     in_, out_);

    // Copy FFTW output buffer back to std::vector
    auto out_begin_ = reinterpret_cast<std::complex<double>*>(out_);
    std::copy(out_begin_, out_begin_ + size_, out.begin());
}

// FFT1D Implementation
FFT1D::FFT1D(int size) : FFT_Base(size, 1) {}

FFT1D::~FFT1D() {}

FFT1D::FFT1D(FFT1D&& other) noexcept : FFT_Base(std::move(other)) {}

FFT1D& FFT1D::operator=(FFT1D&& other) noexcept {
    FFT_Base::operator=(std::move(other));
    return *this;
}

// FFT2D Implementation
FFT2D::FFT2D(int nx, int ny) : FFT_Base(nx * ny, 2), nx_(nx), ny_(ny) {}

FFT2D::~FFT2D() {}

FFT2D::FFT2D(FFT2D&& other) noexcept 
    : FFT_Base(std::move(other)), nx_(other.nx_), ny_(other.ny_) {
    other.nx_ = 0;
    other.ny_ = 0;
}

FFT2D& FFT2D::operator=(FFT2D&& other) noexcept {
    if (this != &other) {
        FFT_Base::operator=(std::move(other));
        nx_ = other.nx_;
        ny_ = other.ny_;
        other.nx_ = 0;
        other.ny_ = 0;
    }
    return *this;
}

// FFT3D Implementation
FFT3D::FFT3D(int nx, int ny, int nz) : FFT_Base(nx * ny * nz, 3), nx_(nx), ny_(ny), nz_(nz) {}

FFT3D::~FFT3D() {}

FFT3D::FFT3D(FFT3D&& other) noexcept 
    : FFT_Base(std::move(other)), nx_(other.nx_), ny_(other.ny_), nz_(other.nz_) {
    other.nx_ = 0;
    other.ny_ = 0;
    other.nz_ = 0;
}

FFT3D& FFT3D::operator=(FFT3D&& other) noexcept {
    if (this != &other) {
        FFT_Base::operator=(std::move(other));
        nx_ = other.nx_;
        ny_ = other.ny_;
        nz_ = other.nz_;
        other.nx_ = 0;
        other.ny_ = 0;
        other.nz_ = 0;
    }
    return *this;
}


} // namespace fft
} // namespace dsp