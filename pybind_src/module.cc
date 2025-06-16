#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include "dsp/convolve.h"
#include "dsp/fft.h"
#include "dsp/polyphase.h"

namespace py = pybind11;

PYBIND11_MODULE(signal_sniper_py, m) {
    py::class_<dsp::convolve::Convolve>(m, "Convolve")
        .def(py::init<const cfloat_vec&, bool, bool>(), 
             py::arg("filter"), py::arg("is_corelation") = false, py::arg("measure_best_fft_size") = true)
        .def(py::init<int, bool>(), 
             py::arg("size"), py::arg("measure_best_fft_size") = true)
        .def("overlap_save",
             &dsp::convolve::Convolve::overlap_save, 
             py::arg("input"), py::arg("propogate_delay") = false)
        .def("convolve", 
             &dsp::convolve::Convolve::convolve, 
             py::arg("input"), py::arg("filter"), py::arg("propogate_delay"))
        .def("correlate", 
             &dsp::convolve::Convolve::correlate, 
             py::arg("input"), py::arg("filter"));

    m.def("convolve", 
          &dsp::convolve::convolve, 
          py::arg("input"), py::arg("filter"), 
          py::arg("propogate_delay") = true, py::arg("fft_overlap_save") = true);

    m.def("correlate", 
          &dsp::convolve::correlate, 
          py::arg("input"), py::arg("filter"), 
          py::arg("fft_overlap_save") = true);

    // Expose FFT1D function
    m.def("fft1d", 
          [](const cfloat_vec& input, bool inverse) {
              int sign = inverse ? FFTW_BACKWARD : FFTW_FORWARD;
              return dsp::fft::fft1d(input, sign);
          }, py::arg("input"), py::arg("inverse") = false);

    // Expose FFT2D function
    m.def("fft2d", 
          [](const cfloat_vec& input, bool inverse, int nx, int ny) {
              int sign = inverse ? FFTW_BACKWARD : FFTW_FORWARD;
              return dsp::fft::fft2d(input, sign, nx, ny);
          }, py::arg("input"), py::arg("inverse") = false, py::arg("nx"), py::arg("ny"));

    // Expose FFT1D class
    py::class_<dsp::fft::FFT1D>(m, "FFT1D")
        .def(py::init<int>(), py::arg("size"))
        .def("execute", 
             [](dsp::fft::FFT1D& self, const cfloat_vec& input, bool inverse) {
                 int sign = inverse ? FFTW_BACKWARD : FFTW_FORWARD;
                 cfloat_vec output(input.size());
                 self.execute(input, output, sign);
                 return output;
             }, py::arg("input"), py::arg("inverse") = false);

    // Expose Polyphase class
    py::class_<dsp::polyphase::Polyphase>(m, "Polyphase")
        .def(py::init<int, int>(), py::arg("factor"), py::arg("num_taps"))
        .def(py::init<int, cfloat_vec>(), py::arg("factor"), py::arg("coeffs"))
        .def("interpolate", &dsp::polyphase::Polyphase::interpolate, py::arg("input"))
        .def("decimate", &dsp::polyphase::Polyphase::decimate, py::arg("input"));

     // Expose convolve_stride function
     m.def("convolve_stride", 
     [](const cfloat_vec& input, const cfloat_vec& filter, int input_stride, int filter_stride, bool conjugate) {
          cfloat_vec output(input.size() / input_stride, cfloat(0.0, 0.0));
          dsp::convolve::convolve_stride(input.begin(), input.end(), filter.begin(), filter.end(), output.begin(), output.end(), input_stride, filter_stride, 1, std::floor(filter.size()/filter_stride/2.0f), conjugate);
          return output;
     }, py::arg("input"), py::arg("filter"), py::arg("input_stride"), py::arg("filter_stride"), py::arg("conjugate"));
}