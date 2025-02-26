#ifndef CHANNELIZE_H
#define CHANNELIZE_H

#include <vector>
#include <complex>
#include <Eigen/Dense>
#include "utils/macros.h"
#include "utils/vector_utils.h"
#include "dsp/fft.h"
#include "dsp/tune.h"
#include "dsp/polyphase.h"

namespace dsp {
namespace channelize {

class PolyphaseChannelizer {
public:
    PolyphaseChannelizer(int factor, int num_taps);

    cdouble_vec channelize(const cdouble_vec& input);

private:
    dsp::polyphase::Polyphase polyphase;
    dsp::fft::FFT1D fft;
    int factor;
};

class PolyphaseSynthesizer {
public:
    PolyphaseSynthesizer(int factor, int num_taps);

    cdouble_vec synthesize(const cdouble_vec& input);

private:
    dsp::polyphase::Polyphase polyphase;
    dsp::fft::FFT1D fft;
    int factor;
};

class Channelizer {
public:
    Channelizer(int sample_rate, double center_frequency, int num_taps, int dds_bit_precision, int buffer_size);

    void fill_buffer(const cdouble_vec& input);

    cdouble_vec channelize(double frequency, int decimation_factor);

    void create_polyphase_filter(int factor);

private:
    int sample_rate;
    double center_frequency;
    dsp::tune::DDS dds;
    int num_taps;
    int buffer_size;
    std::vector<dsp::polyphase::Polyphase> polyphase_filters;
    cdouble_vec buffer;
    int factor;

    dsp::polyphase::Polyphase* find_or_create_polyphase_filter(int factor);
};

class Synthesizer {
public:
    Synthesizer(int sample_rate, double center_frequency, int num_taps, int dds_bit_precision, int buffer_size);

    void synthesize(const cdouble_vec& input, double frequency, int interpolation_factor);

    const cdouble_vec& get_buffer() const;

    void create_polyphase_filter(int factor);

private:
    int sample_rate;
    double center_frequency;
    dsp::tune::DDS dds;
    int num_taps;
    int buffer_size;
    std::vector<dsp::polyphase::Polyphase> polyphase_filters;
    cdouble_vec buffer;
    int factor;

    dsp::polyphase::Polyphase* find_or_create_polyphase_filter(int factor);
};

} // namespace channelize
} // namespace dsp

#endif // CHANNELIZE_H