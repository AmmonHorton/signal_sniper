#ifndef SINC_H
#define SINC_H

#include <vector>
#include <stdexcept>
#include "convolve.h"

namespace dsp {
namespace sinc {

class LowPassFilter {
public:
    LowPassFilter(int num_taps, double corner_frequency);

    const cdouble_vec& get_filter_coeffs() const;

    cdouble_vec filter(const cdouble_vec& input_signal);

private:
    int num_taps;
    double corner_frequency;
    cdouble_vec filter_coeffs;
    dsp::convolve::Convolve convolver;

    void calculate_filter_coeffs();
};

// Function declarations
cdouble_vec make_sinc(int num_taps, double corner_frequency);
std::vector<double> make_hamming(int num_taps);
void spectral_invert(cdouble_vec& filter_coeffs);

} // namespace sinc
} // namespace dsp

#endif // SINC_H
