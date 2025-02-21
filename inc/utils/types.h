#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <complex>
#include <string>
#include <bitset>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <cmath>

// Type definitions for complex values
using cdouble = std::complex<double>;
using cfloat = std::complex<float>;
using cint16 = std::complex<int16_t>;

// Type definitions for complex vectors
using cdouble_vec = std::vector<cdouble>;
using cfloat_vec = std::vector<cfloat>;
using cint16_vec = std::vector<cint16>;

// Type definition for a vector of bits
using bit_vec = std::vector<bool>;

// Utility functions for conversions

// Convert a string and number of bits to a bit_vec
static inline bit_vec string_to_bit_vec(const std::string& str, size_t num_bits) {
    if (str.size() * 8 < num_bits) {
        throw std::invalid_argument("String does not contain enough bits");
    }
    bit_vec bit_vector(num_bits);
    for (size_t i = 0; i < num_bits; ++i) {
        bit_vector[i] = (str[i / 8] >> (7 - (i % 8))) & 1;
    }
    return bit_vector;
}

// Convert a bit_vec to a string
static inline std::string bit_vec_to_string(const bit_vec& bit_vector) {
    size_t num_bytes = (bit_vector.size() + 7) / 8;
    std::string str(num_bytes, '\0');
    for (size_t i = 0; i < bit_vector.size(); ++i) {
        str[i / 8] |= (bit_vector[i] << (7 - (i % 8)));
    }
    return str;
}

// Convert a complex float or double vector to a complex int16 vector with scaling
template <typename T>
static inline cint16_vec convert_to_cint16(const std::vector<std::complex<T>>& input, T scale) {
    cint16_vec output(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        output[i] = cint16(static_cast<int16_t>(input[i].real() * scale),
                            static_cast<int16_t>(input[i].imag() * scale));
    }
    return output;
}

// Convert a complex int16 vector to a complex float or double vector with scaling
template <typename T>
static inline std::vector<std::complex<T>> convert_from_cint16(const cint16_vec& input, T scale) {
    std::vector<std::complex<T>> output(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        output[i] = std::complex<T>(input[i].real(), input[i].imag()) * 1/scale;
    }
    return output;
}

// Wrapper functions for std::arg and std::conj
template <typename T>
static inline double angle(const std::complex<T>& value) {
    return std::arg(value);
}

template <typename T>
static inline std::complex<T> conj(const std::complex<T>& value) {
    return std::conj(value);
}

#endif // TYPES_H