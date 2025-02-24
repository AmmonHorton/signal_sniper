#ifndef TUNE_H
#define TUNE_H

#include <vector>
#include <complex>
#include <cmath>
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include "utils/types.h"
#include "utils/vector_utils.h"

namespace dsp {
namespace tune {

// Normalize phase to the range (-0.5, 0.5) or (0, 1) if flag is set
inline double normalize_phase(double phase, bool zero_to_one = false) {
    if (zero_to_one) {
        return phase - std::floor(phase);
    } else {
        return phase - std::floor(phase + 0.5);
    }
}

// Compute the complex exponential of a normalized phase
inline cfloat cexp(float normalized_phase) {
    return std::polar(1.0f, static_cast<float>(2.0 * M_PI * normalized_phase));
}

// Tune function
template <typename Iterator>
void tune(Iterator begin, Iterator end, double phase_increment, double starting_phase = 0.0) {
    double phase_accumulator = starting_phase;
    for (auto it = begin; it != end; ++it) {
        phase_accumulator = normalize_phase(phase_accumulator + phase_increment);
        *it *= cexp(static_cast<float>(phase_accumulator));
    }
}

// for generating a complex sinusoid
cdouble_vec generate_complex_sinusoid(int num_samples, double phase_increment, double starting_phase = 0.0) {
    cdouble_vec samples(num_samples);
    double phase_accumulator = starting_phase;
    for (int ii = 0; ii < num_samples; ++ii) {
        samples[ii] = cexp(static_cast<float>(phase_accumulator));
        phase_accumulator = normalize_phase(phase_accumulator + phase_increment);
    }
    return samples;
}

// DDS (Direct Digital Synthesis) class
class DDS {
public:
    DDS(int bit_precision) : bit_precision(bit_precision), lut_size(1 << bit_precision) {
        generate_lut();
    }

    // Generate the LUT
    void generate_lut() {
        lut.resize(lut_size);
        double phase_increment = 1.0 / lut_size;
        double phase = 0.0;
        for (int ii = 0; ii < lut_size; ++ii) {
            lut[ii] = cexp(static_cast<float>(phase));
            phase += phase_increment;
        }
    }

    // Access the LUT
    std::complex<float> cexp(double normalized_phase) const {
        int index = static_cast<int>(normalize_phase(normalized_phase, true) * lut_size);
        return lut[index];
    }

    // Tune a signal using the LUT
    template <typename Iterator>
    void tune(Iterator begin, Iterator end, double phase_increment, double starting_phase = 0.0) const {
        double phase_accumulator = starting_phase;
        for (auto it = begin; it != end; ++it) {
            phase_accumulator += phase_increment;
            int lut_index = static_cast<int>(phase_accumulator * lut_size);
            lut_index &= lut_size - 1;
            *it *= lut[lut_index];
        }
    }

private:
    int bit_precision;
    int lut_size;
    cfloat_vec lut;
};

} // namespace tune
} // namespace dsp

#endif // TUNE_H