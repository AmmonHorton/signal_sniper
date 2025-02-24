#include "dsp/polyphase.h"

namespace dsp {
namespace polyphase {

// Constructor for Polyphase
Polyphase::Polyphase(int factor, int num_taps)
    : factor(factor), num_taps(num_taps) {
    make_filter();
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

    // Slice the filter into N slices
    for (int ii = 0; ii < factor; ++ii) {
        filter_slices.push_back(slice(filter_coeffs, filter_coeffs.begin() + ii, filter_coeffs.end(), factor));
    }
}

// Break input into branches
Eigen::MatrixXcd Polyphase::break_into_branches(const cdouble_vec& input) const {
    int num_branches = factor;
    int branch_size = (input.size() + factor - 1) / factor; // ceil(input.size() / factor)
    Eigen::MatrixXcd branches(num_branches, branch_size);

    // Map the input vector to an Eigen matrix
    Eigen::Map<const Eigen::MatrixXcd> input_matrix(input.data(), branch_size, num_branches);

    // Transpose the input matrix to get the branches
    branches = input_matrix.transpose();

    return branches;
}

// Convolve branches with filter slices
Eigen::MatrixXcd Polyphase::convolve_branches(const Eigen::MatrixXcd& branches) const {
    int num_branches = branches.rows();
    int branch_size = branches.cols();
    Eigen::MatrixXcd convolved_branches(num_branches, branch_size);

    // Convolve each branch with the corresponding filter slice
    for (int i = 0; i < num_branches; ++i) {
        cdouble_vec branch(branches.row(i).data(), branches.row(i).data() + branch_size);
        convolved_branches.row(i) = Eigen::Map<Eigen::VectorXcd>(dsp::convolve::convolve(branch, filter_slices[i], true, false).data(), branch_size);
    }

    return convolved_branches;
}

// Sum down the columns for interpolation
cdouble_vec Polyphase::sum_branches(const Eigen::MatrixXcd& branches) const {
    int output_size = branches.cols();
    cdouble_vec output(output_size, cdouble(0.0, 0.0));

    // Sum down the columns
    for (int j = 0; j < output_size; ++j) {
        for (int i = 0; i < branches.rows(); ++i) {
            output[j] += branches(i, j);
        }
    }

    return output;
}

// Interpolate the input signal
cdouble_vec Polyphase::interpolate(const cdouble_vec& input) const {
    auto branches = break_into_branches(input);
    auto convolved_branches = convolve_branches(branches);
    return sum_branches(convolved_branches);
}

// Decimate the input signal
cdouble_vec Polyphase::decimate(const cdouble_vec& input) const {
    auto branches = break_into_branches(input);
    auto convolved_branches = convolve_branches(branches);
    return sum_branches(convolved_branches);
}

} // namespace polyphase
} // namespace dsp