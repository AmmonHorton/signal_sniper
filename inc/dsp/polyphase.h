#ifndef POLYPHASE_H
#define POLYPHASE_H

#include <cmath>
#include <algorithm>
#include <numeric>
#include <vector>
#include <Eigen/Dense>
#include "dsp/convolve.h"
#include "dsp/sinc.h"
#include "utils/vector_utils.h"

namespace dsp {
namespace polyphase {

class Polyphase {
public:
    Polyphase(int factor, int num_taps);
    ~Polyphase();

    cdouble_vec branch(const cdouble_vec& input) const;
    cdouble_vec interleave(const cdouble_vec& input) const;

    cdouble_vec convolve_branches_decimate(const cdouble_vec& branches) const;
    cdouble_vec convolve_branches_interpolate(const cdouble_vec& branches) const;
    cdouble_vec sum_branches(const cdouble_vec& branches) const;

    cdouble_vec interpolate(const cdouble_vec& input) const;
    cdouble_vec decimate(const cdouble_vec& input) const;

    int get_factor() const { return factor; }

private:
    int factor;
    int num_taps;
    cdouble_vec filter_slices;

    void make_filter();
};

} // namespace polyphase
} // namespace dsp

#endif // POLYPHASE_H