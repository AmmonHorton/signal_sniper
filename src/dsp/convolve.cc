#include "dsp/convolve.h"
#include <algorithm>
#include <iterator>

namespace dsp {
namespace convolve {

Convolve::Convolve(const cdouble_vec& filter, bool is_corelation, bool measure_best_fft_size) : fitler_size_(filter.size()) {
    
    // determine the size of the fft blocks
    if (measure_best_fft_size) {
        fft_block_size_ = dsp::fft::find_optimal_fft_size(fitler_size_ * 8);
    } else {
        fft_block_size_ = fitler_size_ * 8;
    }
    fft_filter_.resize(fft_block_size_);
    create_plans();
    load_filter(filter, is_corelation);
}

Convolve::Convolve(int size, bool measure_best_fft_size) : fitler_size_(size) {
    
    // determine the size of the fft blocks
    if (measure_best_fft_size) {
        fft_block_size_ = dsp::fft::find_optimal_fft_size(fitler_size_ * 8);
    } else {
        fft_block_size_ = fitler_size_ * 8;
    }
    fft_filter_.resize(fft_block_size_);
    create_plans();
}

Convolve::~Convolve() {
    fftw_destroy_plan(plan_forward_);
    fftw_destroy_plan(plan_backward_);
}

void Convolve::create_plans() {
    plan_forward_ = fftw_plan_dft_1d(fft_block_size_, nullptr, nullptr, FFTW_FORWARD, FFTW_ESTIMATE);
    plan_backward_ = fftw_plan_dft_1d(fft_block_size_, nullptr, nullptr, FFTW_BACKWARD, FFTW_ESTIMATE);
    EXPECT_TRUE(plan_forward_ && plan_backward_, "Failed to create FFTW plans");
}

void Convolve::load_filter(const cdouble_vec& filter, bool is_corelation) {
    EXPECT_TRUE(filter.size() <= fft_block_size_, "Filter size must be less than or equal to the fft block size");

    cdouble_vec filter_padded(fft_block_size_, cdouble(0, 0));
    if (is_corelation) {
        std::transform(filter.begin(), filter.end(), filter_padded.rbegin(), [](const cdouble& val) {
            return std::conj(val);
        });
    } else {
        std::copy(filter.begin(), filter.end(), filter_padded.begin());
    }
    fftw_execute_dft(plan_forward_, reinterpret_cast<fftw_complex*>(filter_padded.data()), reinterpret_cast<fftw_complex*>(fft_filter_.data()));
}

cdouble_vec Convolve::overlap_save(const cdouble_vec& input, bool propogate_delay) {
    EXPECT_TRUE(input.size() >= fitler_size_, "Input size must be greater than or equal to the filter size");
    size_t output_size = input.size();

    // if propogate delay is true, make sure the vector we are referencing is bigger
    output_size += (propogate_delay) ? (fitler_size_/2) : 0;
    
    cdouble_vec fft_input = (propogate_delay)? cdouble_vec(output_size, cdouble(0, 0)): input;

    // Then copy the data into the larger vector
    if (propogate_delay) {
        std::copy(input.begin(), input.end(), fft_input.begin() + fitler_size_/2);
    }

    cdouble_vec output(output_size, cdouble(0, 0));
    auto input_iter = fft_input.begin();
    auto output_iter = output.begin();

    cdouble_vec input_block(fft_block_size_);
    cdouble_vec fft_output(fft_block_size_);

    while (input_iter <= fft_input.end()) {
        size_t block_size = std::min(fft_block_size_, static_cast<size_t>(std::distance(input_iter, fft_input.end())));
        
        // Copy data amd perform initial fft
        std::copy(input_iter, input_iter + block_size, input_block.begin());
        std::fill(input_block.begin() + block_size, input_block.end(), cdouble(0, 0));
        fftw_execute_dft(plan_forward_, reinterpret_cast<fftw_complex*>(input_block.data()), reinterpret_cast<fftw_complex*>(fft_output.data()));

        // Perform element-wise multiplication and inverse fft
        std::transform(fft_output.begin(), fft_output.end(), fft_filter_.begin(), fft_output.begin(), std::multiplies<cdouble>());
        fftw_execute_dft(plan_backward_, reinterpret_cast<fftw_complex*>(fft_output.data()), reinterpret_cast<fftw_complex*>(input_block.data()));

        // Put the results in the output vector
        std::move(input_block.begin() + fitler_size_ - 1, input_block.end(), output_iter);
        input_iter += block_size - fitler_size_ + 1;
        output_iter += block_size - fitler_size_ + 1;
    }
    return output;
}

cdouble_vec Convolve::convolve(const cdouble_vec& input, const cdouble_vec& filter, bool propogate_delay) {
    load_filter(filter, false);
    return overlap_save(input, propogate_delay);
}

cdouble_vec Convolve::correlate(const cdouble_vec& input, const cdouble_vec& filter) {
    load_filter(filter, true);
    return overlap_save(input, false);
}

cdouble_vec convolve(const cdouble_vec& input, const cdouble_vec& filter, bool propogate_delay, bool fft_overlap_save) {
    if (fft_overlap_save) {
        Convolve convolver(filter, false, true);
        return convolver.overlap_save(input, propogate_delay);
    } else {
        size_t output_size = (propogate_delay) ? (input.size() + filter.size() / 2) : input.size();
        cdouble_vec output(output_size, cdouble(0, 0));
        size_t input_start =  (propogate_delay) ? filter.size() / 2 : filter.size();
        for (size_t ii = input_start; ii < output_size; ii++) {
            for (size_t jj = 1; jj <= filter.size(); jj++) {
                // This if statement hsould only be untrue when using propogate delay
                if (ii - jj >= 0 && ii - jj < input.size()) {
                    output[ii] += input[ii - jj] * filter[jj];
                }
            }
        }
        return output;
    }
}

cdouble_vec correlate(const cdouble_vec& input, const cdouble_vec& filter, bool fft_overlap_save) {
    if (fft_overlap_save) {
        Convolve convolver(filter, true, true);
        return convolver.correlate(input, filter);
    } else {
        size_t output_size = input.size() - filter.size() - 1;
        cdouble_vec output(output_size, cdouble(0, 0));
        for (size_t ii = filter.size(); ii < output_size; ii++) {
            for (size_t jj = 1; jj <= filter.size(); jj++) {
                // This if statement hsould only be untrue when using propogate delay
                if (ii - jj >= 0 && ii - jj < input.size()) {
                    output[ii] += input[ii] * std::conj(filter[jj]);
                }
            }
        }
        return output;
    }
}

} // namespace convolve
} // namespace dsp