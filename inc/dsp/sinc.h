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

    const cfloat_vec& get_filter_coeffs() const;

    cfloat_vec filter(const cfloat_vec& input_signal);

private:
    int num_taps;
    double corner_frequency;
    cfloat_vec filter_coeffs;
    dsp::convolve::Convolve convolver;

    void calculate_filter_coeffs();
};

// Function declarations
cfloat_vec make_sinc(int num_taps, double corner_frequency);
std::vector<double> make_hamming(int num_taps);
void spectral_invert(cfloat_vec& filter_coeffs);

} // namespace sinc
} // namespace dsp

#endif // SINC_H
