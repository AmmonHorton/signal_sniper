#include "dsp/polyphase.h"

namespace dsp {
namespace polyphase {

// Constructor for Polyphase
Polyphase::Polyphase(int factor, int num_taps)
    : factor(factor), num_taps(num_taps) {
    make_filter();
}

Polyphase::Polyphase(int factor, const cdouble_vec& coeffs)
    : factor(factor), num_taps(coeffs.size()) {
    filter_slices = branch(coeffs);
}

// Destructor for Polyphase
Polyphase::~Polyphase() {}

// Create the filter coefficients
void Polyphase::make_filter() {
    cdouble_vec filter_coeffs = dsp::sinc::make_sinc(num_taps, 1.0 / factor);  // Use 1/factor for both interpolation and decimation
    std::vector<double> hamming_window = dsp::sinc::make_hamming(num_taps);
    for (int ii = 0; ii < num_taps; ++ii) {
        filter_coeffs[ii] *= hamming_window[ii];
    }

    filter_slices = branch(filter_coeffs, true);
}

// Break input into branches
cdouble_vec Polyphase::branch(const cdouble_vec& input, bool flip) const {
    size_t size_per_branch = std::ceil(input.size() / factor);
    cdouble_vec output(size_per_branch * factor, cdouble(0.0, 0.0));
    for (int ii = 0; ii < factor; ++ii) {
        if (flip) {
            slice(input.rbegin() + ii, input.rend(), factor, (output.begin() + (ii*size_per_branch)));
        } else {
            slice(input.begin() + ii, input.end(), factor, (output.begin() + (ii*size_per_branch)));
        }
    }
    return output;
}

// Collpase  a vector of multiple branches into one
cdouble_vec Polyphase::interleave(const cdouble_vec& input) const {
    cdouble_vec output(input.size(), cdouble(0.0, 0.0));
    size_t size_per_branch = std::ceil(input.size() / factor);
    for(int ii = 0; ii < size_per_branch; ++ii) {
        slice(input.begin() + ii, input.end(), size_per_branch, (output.begin() + (ii*factor)));
    }
    return output;
}

cdouble_vec Polyphase::convolve_branches_interpolate(const cdouble_vec& input) const {
    // size_t size_per_input_branch = std::ceil(input.size() / factor);
    size_t size_per_filter_branch = std::ceil(filter_slices.size() / factor);
    cdouble_vec convolved_branches(input.size() * factor);

    // Convolve each branch with the corresponding filter slice
    // TODO Change the convolve to use iterators and strides so that it is not computing unnecessary values
    for (int ii = 0; ii < factor; ++ii) {
        cdouble_vec filter_forward(filter_slices.begin() + (ii*size_per_filter_branch), filter_slices.begin() + ((ii+1)*size_per_filter_branch));
        // cdouble_vec filter(filter_forward.rbegin(), filter_forward.rend());
        auto convolved = dsp::convolve::convolve(input, filter_forward, true, false);
        // slice(convolved.begin() + ii, convolved.end(), factor, (convolved_branches.begin() + (ii*size_per_input_branch)));
        std::copy(convolved.begin(), convolved.end(), convolved_branches.begin() + (ii*input.size())); 
    }

    return convolved_branches;
}

// Convolve branches with filter slices
cdouble_vec Polyphase::convolve_branches_decimate(const cdouble_vec& branches) const {
    size_t size_per_input_branch = std::ceil(branches.size() / factor);
    size_t size_per_filter_branch = std::ceil(filter_slices.size() / factor);
    cdouble_vec convolved_branches(branches.size());

    // Convolve each branch with the corresponding filter slice
    for (int ii = 0; ii < factor; ++ii) {
        cdouble_vec branch(branches.begin() + (ii*size_per_input_branch), branches.begin() + ((ii+1)*size_per_input_branch));
        cdouble_vec filter_forward(filter_slices.begin() + (ii*size_per_filter_branch), filter_slices.begin() + ((ii+1)*size_per_filter_branch));
        cdouble_vec filter(filter_forward.rbegin(), filter_forward.rend());
        auto convolved = dsp::convolve::convolve(branch, filter, true, false);
        std::copy(convolved.begin(), convolved.end(), convolved_branches.begin() + (ii*size_per_input_branch));
    }

    return convolved_branches;
}

// Sum down the columns for decimation
cdouble_vec Polyphase::sum_branches(const cdouble_vec& branches) const {
    int output_size = branches.size() / factor;
    cdouble_vec output(output_size, cdouble(0.0, 0.0));

    // Sum down the columns
    for (int ii = 0; ii < output_size; ii++) {
        for(int jj = 0; jj < factor; jj++) {
            output[ii] += branches[ii + jj*output_size];
        }
    }
    return output;
}

// Interpolate the input signal
cdouble_vec Polyphase::interpolate(const cdouble_vec& input) const {
    auto convolved_branches = convolve_branches_interpolate(input);
    cdouble_vec output = interleave(convolved_branches);
    return output;
}

// Decimate the input signal
cdouble_vec Polyphase::decimate(const cdouble_vec& input) const {
    auto branches = branch(input);
    auto convolved_branches = convolve_branches_decimate(branches);
    return sum_branches(convolved_branches);
}

} // namespace polyphase
} // namespace dsp