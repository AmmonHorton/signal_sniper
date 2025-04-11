#include "dsp/polyphase.h"

namespace dsp {
namespace polyphase {

// Constructor for Polyphase
Polyphase::Polyphase(int factor, int num_taps)
    : factor(factor), num_taps(num_taps) {
    filter_coeffs = dsp::sinc::make_sinc(num_taps, 1.0 / factor);  // Use 1/factor for both interpolation and decimation
    std::vector<double> hamming_window = dsp::sinc::make_hamming(num_taps);
    for (int ii = 0; ii < num_taps; ++ii) {
        filter_coeffs[ii] *= hamming_window[ii];
    }
}

Polyphase::Polyphase(int factor, const cfloat_vec& coeffs)
    : factor(factor), num_taps(coeffs.size()), filter_coeffs(coeffs.begin(), coeffs.end()) {
}

// Destructor for Polyphase
Polyphase::~Polyphase() {}

cfloat_vec Polyphase::interpolate(const cfloat_vec& input) const {
    cfloat_vec output(input.size() * factor, cfloat(0.0, 0.0));
    for (int ii = 0; ii < factor; ii++) {
        dsp::convolve::convolve_stride(input.begin(), input.end(), filter_coeffs.rbegin() + factor - 1 - ii, filter_coeffs.rend(),
                                        output.begin() + ii, output.end(), 1, factor, factor, filter_coeffs.size()/factor/2, false);
    }
    return output;
}

cfloat_vec Polyphase::decimate(const cfloat_vec& input) const {
    size_t branch_size = std::ceil(input.size() / static_cast<float>(factor));
    cfloat_vec output(branch_size, cfloat(0.0, 0.0));
    for (int ii = 0; ii < factor; ii++) {
        dsp::convolve::convolve_stride(input.begin() + ii, input.end(), filter_coeffs.begin() + ii, filter_coeffs.end(),
                                       output.begin(), output.end(), factor, factor, 1, filter_coeffs.size()/factor/2, false);
    }
    return output;
}

} // namespace polyphase
} // namespace dsp