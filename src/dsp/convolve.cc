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
    
    fft_filter_ = cdouble_vec(fft_block_size_, cdouble(0, 0));
    fft_plan_ = std::move(FFT1D(fft_block_size_));
    load_filter(filter, is_corelation);
}

Convolve::Convolve(int size, bool measure_best_fft_size) : fitler_size_(size) {
    
    // determine the size of the fft blocks
    if (measure_best_fft_size) {
        fft_block_size_ = dsp::fft::find_optimal_fft_size(fitler_size_ * 8);
    } else {
        fft_block_size_ = fitler_size_ * 8;
    }
    fft_filter_ = cdouble_vec(fft_block_size_, cdouble(0, 0));
    fft_plan_ = std::move(FFT1D(fft_block_size_));
}


void Convolve::load_filter(const cdouble_vec& filter, bool is_corelation) {
    CHECK_TRUE(filter.size() <= fft_block_size_, "Filter size must be less than or equal to the fft block size");
    CHECK_TRUE(filter.size() > 3, "Filter size must be larger than 3 for overlap save usage");
    
    cdouble_vec filter_padded(fft_block_size_, cdouble(0, 0));
    if (is_corelation) {
        std::transform(filter.rbegin(), filter.rend(), filter_padded.begin(), [](const cdouble& val) {
            return std::conj(val);
        });
    } else {
        std::copy(filter.begin(), filter.end(), filter_padded.begin());
    }
    fft_plan_.execute(filter_padded, fft_filter_, FFTW_FORWARD);
}

cdouble_vec Convolve::overlap_save(const cdouble_vec& input, bool propogate_delay) {
    CHECK_TRUE(input.size() >= fitler_size_, "Input size must be greater than or equal to the filter size");

    size_t overlap = fitler_size_ - 1;
    size_t advance = fft_block_size_ - overlap;

    size_t total_zero_fill = (propogate_delay)? std::floor(fitler_size_/2): 0;
    
    size_t output_size = (propogate_delay)? input.size(): input.size() - fitler_size_ + 1;
    cdouble_vec output(output_size, cdouble(0, 0));
    
    auto input_iter = input.begin();
    auto output_iter = output.begin();
    
    cdouble_vec input_block(fft_block_size_, cdouble(0, 0));
    cdouble_vec fft_output(fft_block_size_);
    
    double scale = 1.0 / fft_block_size_;
    while (input_iter < input.end() && output_iter < output.end()) {
        // Can only zero fill up to one block size
        size_t zero_fill = std::min(total_zero_fill, fft_block_size_);

        // Compute the zero fill and samples to add
        input_iter = (zero_fill)? input.begin(): input_iter;
        size_t samples_to_add = fft_block_size_ - zero_fill;
        samples_to_add = std::min(samples_to_add, static_cast<size_t>(std::distance(input_iter, input.end())));
        
        // Copy the input data into the input block
        CHECK_TRUE(input_iter + samples_to_add <= input.end(), "Input iterator going out of bounds");
        auto input_copied = std::copy(input_iter, input_iter + samples_to_add, input_block.begin() + zero_fill);
        std::fill(input_copied, input_block.end(), cdouble(0, 0));
        
        // Advance counters/iterators
        input_iter += advance;
        total_zero_fill -= std::min(advance, total_zero_fill);
        if (zero_fill < advance) {
            input_iter -= zero_fill;
        }

        // Perform fft, element-wise multiplication, and inverse fft
        fft_plan_.execute(input_block, fft_output, FFTW_FORWARD);
        std::transform(fft_output.begin(), fft_output.end(), fft_filter_.begin(), fft_output.begin(), std::multiplies<cdouble>());
        fft_plan_.execute(fft_output, input_block, FFTW_BACKWARD);

        // Copy the valid samples to the output
        auto valid_sample_start = input_block.begin() + overlap;
        samples_to_add = input_block.end() - valid_sample_start;
        samples_to_add = std::min(samples_to_add, static_cast<size_t>(output.end() - output_iter));

        CHECK_TRUE(output_iter+ samples_to_add <= output.end(), "Output iterator going out of bounds");
        output_iter = std::transform(valid_sample_start, valid_sample_start + samples_to_add, output_iter, [scale](const cdouble& val) {
            return val * scale;
        });

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
        size_t output_size = (propogate_delay)? input.size() : input.size() - filter.size() + 1;
        cdouble_vec output(output_size, cdouble(0, 0));

        int filter_index_start = (propogate_delay)? std::floor(filter.size()/2.0): 0;
        bool done_padding = true;
        size_t input_array_start = 0;
        for (int ii = 0; ii < output_size; ii++) {
            input_array_start += (done_padding)? 0 : 1;
            int max_iter = std::min(filter.size(), input.size() - input_array_start);
            for (int jj = filter_index_start; jj < max_iter; jj++) {
                int input_array_iter = jj - filter_index_start;
                output[ii] += input[input_array_start + input_array_iter] * filter.rbegin()[jj];
            }
            done_padding = static_cast<bool>(filter_index_start);
            filter_index_start -= (filter_index_start)? 1: 0;
        }
        return output;
    }
}

cdouble_vec correlate(const cdouble_vec& input, const cdouble_vec& filter, bool fft_overlap_save) {
    if (fft_overlap_save) {
        Convolve convolver(filter, true, true);
        return convolver.correlate(input, filter);
    } else {
        size_t output_size = input.size() - filter.size() + 1;
        cdouble_vec output(output_size, cdouble(0, 0));
        for (int ii = 0; ii < output_size; ii++) {
            for (int jj = 0; jj < filter.size(); jj++) {
                output[ii] += input[ii + jj] * std::conj(filter[jj]);
            }
        }
        return output;
    }
}

} // namespace convolve
} // namespace dsp
