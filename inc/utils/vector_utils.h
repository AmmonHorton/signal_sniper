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


// Generic slicing function that works for any iterator type
template <typename InputIterator, typename OutputIterator>
void slice(InputIterator start, InputIterator end, int step, OutputIterator out_start) {
    for (auto it = start; it < end; it += step) {
        *out_start++ = *it;
    }
}

// Generic slicing function that returns a vector
template <typename Iterator>
std::vector<typename std::iterator_traits<Iterator>::value_type> 
slice(Iterator start, Iterator end, int step) {
    using T = typename std::iterator_traits<Iterator>::value_type;
    std::vector<T> output;
    for (auto it = start; it < end; it += step) {
        output.push_back(*it);
    }
    return output;
}

// Break input into branches
template <typename T>
std::vector<T> branch(const std::vector<T>& input, int factor) {
    size_t size_per_branch = std::ceil(input.size() / factor);
    std::vector<T> output(size_per_branch * factor, T(0.0, 0.0));
    for (int ii = 0; ii < factor; ++ii) {
        slice(input.begin() + ii, input.end(), factor, (output.begin() + (ii*size_per_branch)));
    }
    return output;
}

// Collpase  a vector of multiple branches into one
template <typename T>
std::vector<T> interleave(const std::vector<T>& input, int factor) {
    std::vector<T> output(input.size(), T(0.0, 0.0));
    size_t size_per_branch = std::ceil(input.size() / factor);
    for(int ii = 0; ii < size_per_branch; ++ii) {
        slice(input.begin() + ii, input.end(), size_per_branch, (output.begin() + (ii*factor)));
    }
    return output;
}

// Sum down the columns af a vector containg branches
template <typename T>
std::vector<T> sum_down_columns(const std::vector<T>& branches, int factor) {
    int output_size = branches.size() / factor;
    std::vector<T> output(output_size, T(0.0, 0.0));

    // Sum down the columns
    for (int ii = 0; ii < output_size; ii++) {
        for(int jj = 0; jj < factor; jj++) {
            output[ii] += branches[ii + jj*output_size];
        }
    }
    return output;
}

#endif // VECTOR_UTILS_H