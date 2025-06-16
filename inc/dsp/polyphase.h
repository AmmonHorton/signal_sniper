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
    Polyphase(int factor, const cfloat_vec& coeffs);
    ~Polyphase();

    cfloat_vec interpolate(const cfloat_vec& input) const;
    cfloat_vec decimate(const cfloat_vec& input) const;

    int get_factor() const { return factor; }
    cfloat_vec get_filter_coeffs() const { return filter_coeffs; }

private:
    int factor;
    int num_taps;
    cfloat_vec filter_coeffs;

    void make_filter();
};

} // namespace polyphase
} // namespace dsp

#endif // POLYPHASE_H