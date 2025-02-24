#ifndef CHANNELIZE_H
#define CHANNELIZE_H

#include <vector>
#include <complex>
#include <Eigen/Dense>
#include "polyphase.h"
#include "fft.h"
#include "tune.h"
#include "utils/macros.h"

namespace dsp {
namespace channelize {

class PolyphaseChannelizer {
public:
    PolyphaseChannelizer(int factor, int num_taps);

    Eigen::MatrixXcd channelize(const cdouble_vec& input);

    int get_factor() const;

private:
    dsp::polyphase::Polyphase polyphase;
    dsp::fft::FFT1D fft;
};

class PolyphaseSynthesizer {
public:
    PolyphaseSynthesizer(int factor, int num_taps);

    cdouble_vec synthesize(const std::vector<cdouble_vec>& input);

    int get_factor() const;

private:
    dsp::polyphase::Polyphase polyphase;
    dsp::fft::FFT1D fft;
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

    dsp::polyphase::Polyphase* find_or_create_polyphase_filter(int factor);
};

} // namespace channelize
} // namespace dsp

#endif // CHANNELIZE_H