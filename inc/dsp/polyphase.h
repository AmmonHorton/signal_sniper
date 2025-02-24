#ifndef POLYPHASE_H
#define POLYPHASE_H

#include <cmath>
#include <algorithm>
#include <vector>
#include <Eigen/Dense>
#include "convolve.h"
#include "sinc.h"
#include "utils/vector_utils.h"

namespace dsp {
namespace polyphase {

class Polyphase {
public:
    Polyphase(int factor, int num_taps);
    ~Polyphase();

    Eigen::MatrixXcd break_into_branches(const cdouble_vec& input) const;
    Eigen::MatrixXcd convolve_branches(const Eigen::MatrixXcd& branches) const;
    cdouble_vec sum_branches(const Eigen::MatrixXcd& branches) const;

    cdouble_vec interpolate(const cdouble_vec& input) const;
    cdouble_vec decimate(const cdouble_vec& input) const;

    int get_factor() const { return factor; }

private:
    int factor;
    int num_taps;
    std::vector<cdouble_vec> filter_slices;

    void make_filter();
};

} // namespace polyphase
} // namespace dsp

#endif // POLYPHASE_H