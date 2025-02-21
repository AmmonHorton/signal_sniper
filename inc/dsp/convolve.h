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
class Convolve {
public:
    // Constructor accepting a filter vector
    Convolve(const cdouble_vec& filter, bool is_corelation = false, bool measure_best_fft_size = true);

    // Constructor accepting a size
    Convolve(int size, bool measure_best_fft_size = true);

    // Destructor
    ~Convolve();

    // Overlap-save convolution
    cdouble_vec overlap_save(const cdouble_vec& input, bool propogate_delay = false);
    
    // Convolve function
    cdouble_vec convolve(const cdouble_vec& input, const cdouble_vec& filter, bool propogate_delay);
    
    // Correlate function
    cdouble_vec correlate(const cdouble_vec& input, const cdouble_vec& filter);
    
    private:
    size_t fitler_size_;
    size_t fft_block_size_;
    fftw_plan plan_forward_;
    fftw_plan plan_backward_;
    cdouble_vec fft_filter_;
    cdouble_vec overlap_;
    
    // Load a new filter
    void load_filter(const cdouble_vec& filter, bool is_corelation);
    
    // Create FFTW plans
    void create_plans();
};

// Outside class functions
cdouble_vec convolve(const cdouble_vec& input, const cdouble_vec& filter, bool propogate_delay = true, bool fft_overlap_save = true);
cdouble_vec correlate(const cdouble_vec& input, const cdouble_vec& filter, bool fft_overlap_save = true);

} // namespace convolve
} // namespace dsp

#endif // CONVOLVE_H