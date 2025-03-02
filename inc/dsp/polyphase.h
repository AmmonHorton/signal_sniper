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
    Polyphase(int factor, const cdouble_vec& coeffs);
    ~Polyphase();

    cdouble_vec interpolate(const cdouble_vec& input) const;
    cdouble_vec decimate(const cdouble_vec& input) const;

    int get_factor() const { return factor; }
    cdouble_vec get_filter_coeffs() const { return filter_coeffs; }

private:
    int factor;
    int num_taps;
    cdouble_vec filter_coeffs;

    void make_filter();
};

} // namespace polyphase
} // namespace dsp

#endif // POLYPHASE_H