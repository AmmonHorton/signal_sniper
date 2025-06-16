#include "dsp/sinc.h"
#include <cmath>
#include <algorithm>

namespace dsp {
namespace sinc {

// Constructor for LowPassFilter
LowPassFilter::LowPassFilter(int num_taps, double corner_frequency)
    : num_taps(num_taps), corner_frequency(corner_frequency), convolver(num_taps) {
    calculate_filter_coeffs();
    convolver.load_filter(filter_coeffs);
}

// Get the filter coefficients
const cfloat_vec& LowPassFilter::get_filter_coeffs() const {
    return filter_coeffs;
}

// Filter the input signal
cfloat_vec LowPassFilter::filter(const cfloat_vec& input_signal) {
    return convolver.overlap_save(input_signal, true);
}

// Calculate the filter coefficients
void LowPassFilter::calculate_filter_coeffs() {
    filter_coeffs = make_sinc(num_taps, corner_frequency);
    std::vector<double> hamming_window = make_hamming(num_taps);
    for (int ii = 0; ii < num_taps; ++ii) {
        filter_coeffs[ii] *= hamming_window[ii];
    }
}

// Create a sinc function
cfloat_vec make_sinc(int num_taps, double corner_frequency) {
    cfloat_vec sinc(num_taps);
    double M = num_taps - 1;
    double fc = corner_frequency;
    for (int n = 0; n < num_taps; ++n) {
        if (n == M / 2) {
            sinc[n] = cfloat(2 * fc, 0.0);
        } else {
            sinc[n] = cfloat(sin(2 * M_PI * fc * (n - M / 2)) / (M_PI * (n - M / 2)), 0.0);
        }
    }
    return sinc;
}

// Create a Hamming window
std::vector<double> make_hamming(int num_taps) {
    std::vector<double> hamming(num_taps);
    double M = num_taps - 1;
    for (int n = 0; n < num_taps; ++n) {
        hamming[n] = 0.54 - 0.46 * cos(2 * M_PI * n / M);
    }
    return hamming;
}

// Spectral inversion
void spectral_invert(cfloat_vec& filter_coeffs) {
    for (int i = 0; i < filter_coeffs.size(); ++i) {
        filter_coeffs[i] = -filter_coeffs[i];
    }
    filter_coeffs[filter_coeffs.size() / 2] += 1;
}

} // namespace sinc
} // namespace dsp