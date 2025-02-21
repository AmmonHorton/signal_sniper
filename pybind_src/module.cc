#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include "dsp/convolve.h"

namespace py = pybind11;

using cdouble = std::complex<double>;
using cdouble_vec = std::vector<cdouble>;

cdouble_vec numpy_to_cdouble_vec(py::array_t<std::complex<double>> array) {
    py::buffer_info buf = array.request();
    cdouble* ptr = static_cast<cdouble*>(buf.ptr);
    return cdouble_vec(ptr, ptr + buf.size);
}

py::array_t<std::complex<double>> cdouble_vec_to_numpy(const cdouble_vec& vec) {
    return py::array_t<std::complex<double>>(vec.size(), vec.data());
}

PYBIND11_MODULE(signal_sniper_python, m) {
    py::class_<dsp::convolve::Convolve>(m, "Convolve")
        .def(py::init<const cdouble_vec&, bool, bool>(), py::arg("filter"), py::arg("is_corelation") = false, py::arg("measure_best_fft_size") = true)
        .def(py::init<int, bool>(), py::arg("size"), py::arg("measure_best_fft_size") = true)
        .def("overlap_save", [](dsp::convolve::Convolve& self, py::array_t<std::complex<double>> input, bool propogate_delay) {
            cdouble_vec input_vec = numpy_to_cdouble_vec(input);
            cdouble_vec result = self.overlap_save(input_vec, propogate_delay);
            return cdouble_vec_to_numpy(result);
        }, py::arg("input"), py::arg("propogate_delay") = false)
        .def("convolve", [](dsp::convolve::Convolve& self, py::array_t<std::complex<double>> input, py::array_t<std::complex<double>> filter, bool propogate_delay) {
            cdouble_vec input_vec = numpy_to_cdouble_vec(input);
            cdouble_vec filter_vec = numpy_to_cdouble_vec(filter);
            cdouble_vec result = self.convolve(input_vec, filter_vec, propogate_delay);
            return cdouble_vec_to_numpy(result);
        }, py::arg("input"), py::arg("filter"), py::arg("propogate_delay"))
        .def("correlate", [](dsp::convolve::Convolve& self, py::array_t<std::complex<double>> input, py::array_t<std::complex<double>> filter) {
            cdouble_vec input_vec = numpy_to_cdouble_vec(input);
            cdouble_vec filter_vec = numpy_to_cdouble_vec(filter);
            cdouble_vec result = self.correlate(input_vec, filter_vec);
            return cdouble_vec_to_numpy(result);
        }, py::arg("input"), py::arg("filter"));

    m.def("convolve", [](py::array_t<std::complex<double>> input, py::array_t<std::complex<double>> filter, bool propogate_delay, bool fft_overlap_save) {
        cdouble_vec input_vec = numpy_to_cdouble_vec(input);
        cdouble_vec filter_vec = numpy_to_cdouble_vec(filter);
        cdouble_vec result = dsp::convolve::convolve(input_vec, filter_vec, propogate_delay, fft_overlap_save);
        return cdouble_vec_to_numpy(result);
    }, py::arg("input"), py::arg("filter"), py::arg("propogate_delay") = true, py::arg("fft_overlap_save") = true);

    m.def("correlate", [](py::array_t<std::complex<double>> input, py::array_t<std::complex<double>> filter, bool fft_overlap_save) {
        cdouble_vec input_vec = numpy_to_cdouble_vec(input);
        cdouble_vec filter_vec = numpy_to_cdouble_vec(filter);
        cdouble_vec result = dsp::convolve::correlate(input_vec, filter_vec, fft_overlap_save);
        return cdouble_vec_to_numpy(result);
    }, py::arg("input"), py::arg("filter"), py::arg("fft_overlap_save") = true);
}