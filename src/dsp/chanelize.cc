#include "dsp/channelize.h"

namespace dsp {
namespace channelize {

// PolyphaseChannelizer Implementation
PolyphaseChannelizer::PolyphaseChannelizer(int factor, int num_taps)
    : factor(factor), polyphase(factor, num_taps), fft(factor) {}

cfloat_vec PolyphaseChannelizer::channelize(const cfloat_vec& input) {
    // cfloat_vec branches = polyphase.branch(input);
    // cfloat_vec convolved_branches = polyphase.convolve_branches_decimate(branches);
    // size_t size_per_input_branch = std::ceil(branches.size() / factor);
    // // Perform FFT on each branch
    // for (int ii = 0; ii < size_per_input_branch; ++ii) {
    //     cfloat_vec column = slice(convolved_branches.begin() + ii, convolved_branches.end(), size_per_input_branch);
    //     cfloat_vec fft_result(column.size());
    //     fft.execute(column, fft_result, FFTW_FORWARD);
    //     std::copy(fft_result.begin(), fft_result.end(), output.begin() + ii*factor);    
    // }
    // output = polyphase.interleave(output);
    cfloat_vec output(input.size());
    return output;
}

// PolyphaseSynthesizer Implementation
PolyphaseSynthesizer::PolyphaseSynthesizer(int factor, int num_taps)
    : polyphase(factor, num_taps), fft(factor) {}

cfloat_vec PolyphaseSynthesizer::synthesize(const cfloat_vec& input) {

    // cfloat_vec cols(input.size());
    // size_t size_per_input_branch = std::ceil(input.size() / factor);
    // for (int ii = 0; ii < size_per_input_branch; ++ii) {
    //     cfloat_vec ifft_input = slice(input.begin() + ii, input.end(), size_per_input_branch);
    //     cfloat_vec ifft_result(input.size());
    //     fft.execute(ifft_input, ifft_result, FFTW_BACKWARD);
    //     std::copy(ifft_result.begin(), ifft_result.end(), cols.begin() + ii*factor);
    // }

    // cfloat_vec rows = polyphase.interleave(cols);
    // cfloat_vec convolved = polyphase.convolve_branches_interpolate(rows);
    // cfloat_vec output = polyphase.interleave(convolved);
    cfloat_vec output(input.size());
    return output;
}

// Channelizer Implementation
Channelizer::Channelizer(int sample_rate, double center_frequency, int num_taps, int dds_bit_precision, int buffer_size)
    : sample_rate(sample_rate), center_frequency(center_frequency), dds(dds_bit_precision), num_taps(num_taps), buffer_size(buffer_size) {}

void Channelizer::fill_buffer(const cfloat_vec& input) {
    buffer.insert(buffer.end(), input.begin(), input.end());
}

cfloat_vec Channelizer::channelize(double frequency, int decimation_factor) {
    CHECK_TRUE(decimation_factor > 0, "Decimation factor must be positive");
    CHECK_TRUE(frequency >= -sample_rate / 2 && frequency <= sample_rate / 2, "Frequency out of range");

    auto polyphase_filter = find_or_create_polyphase_filter(decimation_factor);

    dds.tune(buffer.begin(), buffer.end(), frequency / sample_rate, 1.0 / decimation_factor);
    return polyphase_filter->decimate(buffer);
}

void Channelizer::create_polyphase_filter(int factor) {
    if (std::none_of(polyphase_filters.begin(), polyphase_filters.end(),
                     [factor](const dsp::polyphase::Polyphase& filter) { return filter.get_factor() == factor; })) {
        polyphase_filters.emplace_back(factor, num_taps);
    }
}

dsp::polyphase::Polyphase* Channelizer::find_or_create_polyphase_filter(int factor) {
    auto polyphase_filter = std::find_if(polyphase_filters.begin(), polyphase_filters.end(),
                                         [factor](const dsp::polyphase::Polyphase& filter) {
                                             return filter.get_factor() == factor;
                                         });

    if (polyphase_filter == polyphase_filters.end()) {
        polyphase_filters.emplace_back(factor, num_taps);
        polyphase_filter = std::prev(polyphase_filters.end());
    }

    return &(*polyphase_filter);
}

// Synthesizer Implementation
Synthesizer::Synthesizer(int sample_rate, double center_frequency, int num_taps, int dds_bit_precision, int buffer_size)
    : sample_rate(sample_rate), center_frequency(center_frequency), dds(dds_bit_precision), num_taps(num_taps), buffer_size(buffer_size) {}

void Synthesizer::synthesize(const cfloat_vec& input, double frequency, int interpolation_factor) {
    CHECK_TRUE(interpolation_factor > 0, "Interpolation factor must be positive");
    CHECK_TRUE(frequency >= -sample_rate / 2 && frequency <= sample_rate / 2, "Frequency out of range");

    auto polyphase_filter = find_or_create_polyphase_filter(interpolation_factor);

    auto synthesized_signal = polyphase_filter->interpolate(input);
    dds.tune(synthesized_signal.begin(), synthesized_signal.end(), frequency / sample_rate, 1.0 / interpolation_factor);

    if (buffer.size() < synthesized_signal.size()) {
        buffer.resize(synthesized_signal.size(), cfloat(0.0, 0.0));
    }

    for (size_t i = 0; i < synthesized_signal.size(); ++i) {
        buffer[i] += synthesized_signal[i];
    }
}

const cfloat_vec& Synthesizer::get_buffer() const {
    return buffer;
}

void Synthesizer::create_polyphase_filter(int factor) {
    if (std::none_of(polyphase_filters.begin(), polyphase_filters.end(),
                     [factor](const dsp::polyphase::Polyphase& filter) { return filter.get_factor() == factor; })) {
        polyphase_filters.emplace_back(factor, num_taps);
    }
}

dsp::polyphase::Polyphase* Synthesizer::find_or_create_polyphase_filter(int factor) {
    auto polyphase_filter = std::find_if(polyphase_filters.begin(), polyphase_filters.end(),
                                         [factor](const dsp::polyphase::Polyphase& filter) {
                                             return filter.get_factor() == factor;
                                         });

    if (polyphase_filter == polyphase_filters.end()) {
        polyphase_filters.emplace_back(factor, num_taps);
        polyphase_filter = std::prev(polyphase_filters.end());
    }

    return &(*polyphase_filter);
}

} // namespace channelize
} // namespace dsp