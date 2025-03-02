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
    Convolve(const cdouble_vec& filter, bool is_corelation = false, bool measure_best_fft_size = true);

    // Constructor accepting a size
    Convolve(int size, bool measure_best_fft_size = true);

    // Load a new filter
    void load_filter(const cdouble_vec& filter, bool is_corelation = false);

    // Overlap-save convolution
    cdouble_vec overlap_save(const cdouble_vec& input, bool propogate_delay = false);
    
    // Convolve function
    cdouble_vec convolve(const cdouble_vec& input, const cdouble_vec& filter, bool propogate_delay);
    
    // Correlate function
    cdouble_vec correlate(const cdouble_vec& input, const cdouble_vec& filter);
    

    private:
    size_t fitler_size_;
    size_t fft_block_size_;
    FFT1D fft_plan_;
    cdouble_vec fft_filter_;
    cdouble_vec overlap_;
    
    
    // Create FFTW plans
    void create_plans();
};

// Outside class functions
cdouble_vec convolve(const cdouble_vec& input, const cdouble_vec& filter, bool propogate_delay = true, bool fft_overlap_save = true);
cdouble_vec correlate(const cdouble_vec& input, const cdouble_vec& filter, bool fft_overlap_save = true);


// using cd_iter = cdouble_vec::const_iterator;
template <typename First_Iter, typename Second_Iter, typename Third_Iter>
// template <typename Second_Iter>
static inline void convolve_stride(First_Iter input_begin, First_Iter input_end,
                                          Second_Iter filter_begin, Second_Iter filter_end,
                                          Third_Iter output_begin, Third_Iter output_end, 
                                          int input_stride, int filter_stride, int output_stride,
                                          int delay, bool conjugate) {
    int filter_index_start = delay;
    bool done_padding = true;
    size_t input_array_start = 0;

    int ii = 0;
    using ValueType = typename std::iterator_traits<First_Iter>::value_type;
    while ((input_begin + input_array_start) < (input_end-delay)) {  // Iterate over input
        
        std::vector<ValueType> temp1;
        std::vector<ValueType> temp2;



        auto f_s = filter_begin + filter_index_start * filter_stride;  // Copy filter_begin (removes const restriction)
        input_array_start += (done_padding)? 0 : input_stride;
        int jj = input_array_start;
        while (f_s < filter_end) {  // Iterate over filter
            auto filter_element = (conjugate) ? std::conj(*f_s) : *f_s;
            if (input_begin + jj >= input_end || output_begin + ii >= output_end) {
                break;
            }
            temp1.push_back(input_begin[jj]);
            temp2.push_back(filter_element);
            output_begin[ii] += input_begin[jj] * filter_element;  // Use dereferencing
            
            f_s += filter_stride;  // Increment filter iterator
            jj += input_stride;  // Update input index
        }
        // std::cout << "loop vals: ";
        // for (auto& val : temp1) {
        //     std::cout << static_cast<int>(val.real()) << ", ";
        // }
        // std::cout << "\nFilter vals";
        // for (auto& val : temp2) {
        //     std::cout << static_cast<int>(val.real()) << ", ";
        // }
        // std::cout << "\n";
        

        done_padding = static_cast<bool>(filter_index_start);
        filter_index_start -= (filter_index_start)? 1: 0;

        ii += output_stride;
    }


    // int filter_index_start = (propogate_delay)? std::floor(filter.size()/2.0): 0;
    // bool done_padding = true;
    // size_t input_array_start = 0;
    // for (int ii = 0; ii < output_size; ii++) {
    //     input_array_start += (done_padding)? 0 : 1;
    //     int max_iter = std::min(filter.size(), input.size() - input_array_start);
    //     for (int jj = filter_index_start; jj < max_iter; jj++) {
    //         int input_array_iter = jj - filter_index_start;
    //         output[ii] += input[input_array_start + input_array_iter] * filter.rbegin()[jj];
    //     }
    //     done_padding = static_cast<bool>(filter_index_start);
    //     filter_index_start -= (filter_index_start)? 1: 0;
    // }
}

} // namespace convolve
} // namespace dsp

#endif // CONVOLVE_H