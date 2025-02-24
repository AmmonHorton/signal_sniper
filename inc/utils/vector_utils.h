#ifndef VECTOR_UTILS_H
#define VECTOR_UTILS_H

#include <vector>
#include <complex>
#include <iterator>
#include <cmath>
#include <algorithm>
#include <stdexcept>

// Type definitions for complex values
using cdouble = std::complex<double>;
using cfloat = std::complex<float>;
using cint16 = std::complex<int16_t>;

// Convert a complex vector to a real vector
template <typename T>
std::vector<T> to_real(const std::vector<std::complex<T>>& input) {
    std::vector<T> output(input.size());
    std::transform(input.begin(), input.end(), output.begin(), [](const std::complex<T>& val) {
        return val.real();
    });
    return output;
}

// Convert a complex vector to an imaginary vector
template <typename T>
std::vector<T> to_imag(const std::vector<std::complex<T>>& input) {
    std::vector<T> output(input.size());
    std::transform(input.begin(), input.end(), output.begin(), [](const std::complex<T>& val) {
        return val.imag();
    });
    return output;
}

// Apply the conjugate operation to an entire complex vector
template <typename T>
std::vector<std::complex<T>> conj(const std::vector<std::complex<T>>& input) {
    std::vector<std::complex<T>> output(input.size());
    std::transform(input.begin(), input.end(), output.begin(), [](const std::complex<T>& val) {
        return std::conj(val);
    });
    return output;
}

// Compute the angle of complex numbers in a vector
template <typename T>
std::vector<T> angle(const std::vector<std::complex<T>>& input) {
    std::vector<T> output(input.size());
    std::transform(input.begin(), input.end(), output.begin(), [](const std::complex<T>& val) {
        return std::arg(val);
    });
    return output;
}

// Scale a complex vector by a scalar
template <typename T>
std::vector<std::complex<T>> operator*(const std::vector<std::complex<T>>& input, T scalar) {
    std::vector<std::complex<T>> output(input.size());
    std::transform(input.begin(), input.end(), output.begin(), [scalar](const std::complex<T>& val) {
        return val * scalar;
    });
    return output;
}

// Element-wise multiply two complex vectors
template <typename T>
std::vector<std::complex<T>> operator*(const std::vector<std::complex<T>>& lhs, const std::vector<std::complex<T>>& rhs) {
    if (lhs.size() != rhs.size()) {
        throw std::invalid_argument("Vectors must be of the same size for element-wise multiplication");
    }
    std::vector<std::complex<T>> output(lhs.size());
    std::transform(lhs.begin(), lhs.end(), rhs.begin(), output.begin(), std::multiplies<std::complex<T>>());
    return output;
}

// Slice a vector with a given start iterator, end iterator, and step factor
template <typename T>
std::vector<T> slice(const std::vector<T>& input, typename std::vector<T>::const_iterator start, typename std::vector<T>::const_iterator end, int step) {
    std::vector<T> output;
    for (auto it = start; it < end; it += step) {
        output.push_back(*it);
    }
    return output;
}

#endif // VECTOR_UTILS_H