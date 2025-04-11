#ifndef CONVOLVE_H
#define CONVOLVE_H

#include <vector>
#include <complex>
#include <fftw3.h>
#include "utils/macros.h"
#include "utils/types.h"
#include "dsp/fft.h"

namespace dsp {
namespace convolve {

using dsp::fft::FFT1D;

class Convolve {
public:
    // Constructor accepting a filter vector
    Convolve(const cfloat_vec& filter, bool is_corelation = false, bool measure_best_fft_size = true);

    // Constructor accepting a size
    Convolve(int size, bool measure_best_fft_size = true);

    // Load a new filter
    void load_filter(const cfloat_vec& filter, bool is_corelation = false);

    // Overlap-save convolution
    cfloat_vec overlap_save(const cfloat_vec& input, bool propogate_delay = false);
    
    // Convolve function
    cfloat_vec convolve(const cfloat_vec& input, const cfloat_vec& filter, bool propogate_delay);
    
    // Correlate function
    cfloat_vec correlate(const cfloat_vec& input, const cfloat_vec& filter);
    

    private:
    size_t fitler_size_;
    size_t fft_block_size_;
    FFT1D fft_plan_;
    cfloat_vec fft_filter_;
    cfloat_vec overlap_;
    
    
    // Create FFTW plans
    void create_plans();
};

// Outside class functions
cfloat_vec convolve(const cfloat_vec& input, const cfloat_vec& filter, bool propogate_delay = true, bool fft_overlap_save = true);
cfloat_vec correlate(const cfloat_vec& input, const cfloat_vec& filter, bool fft_overlap_save = true);


template <typename InputIter, typename FilterIter, typename OutputIter>
static inline void convolve_stride(InputIter input_begin, InputIter input_end,
                                   FilterIter filter_begin, FilterIter filter_end,
                                   OutputIter output_begin, OutputIter output_end, 
                                   int input_stride, int filter_stride, int output_stride,
                                   int delay, bool conjugate) {
    int filter_index_start = delay;
    size_t input_index_start = 0;

    for (int ii = 0; ii < std::distance(output_begin, output_end); ii+=output_stride) {  // Iterate over input
        auto f_s = filter_begin + filter_index_start * filter_stride;
        int jj = input_index_start;
        while (f_s < filter_end && input_begin + jj < input_end) {  // Iterate over filter
            auto filter_element = (conjugate) ? std::conj(*f_s) : *f_s;
            output_begin[ii] += input_begin[jj] * filter_element;
            
            f_s += filter_stride;  // Increment filter iterator
            jj += input_stride;  // Update relative input index
        }
        
        input_index_start += (filter_index_start)? 0 : input_stride;
        filter_index_start -= (filter_index_start)? 1: 0;
    }
}

} // namespace convolve
} // namespace dsp

#endif // CONVOLVE_H