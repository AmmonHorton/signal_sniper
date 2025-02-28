#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include "dsp/convolve.h"
#include "dsp/fft.h"
#include "dsp/polyphase.h"

namespace py = pybind11;

PYBIND11_MODULE(signal_sniper_python, m) {
    py::class_<dsp::convolve::Convolve>(m, "Convolve")
        .def(py::init<const cdouble_vec&, bool, bool>(), 
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
          [](const cdouble_vec& input, bool inverse) {
              int sign = inverse ? FFTW_BACKWARD : FFTW_FORWARD;
              return dsp::fft::fft1d(input, sign);
          }, py::arg("input"), py::arg("inverse") = false);

    // Expose FFT2D function
    m.def("fft2d", 
          [](const cdouble_vec& input, bool inverse, int nx, int ny) {
              int sign = inverse ? FFTW_BACKWARD : FFTW_FORWARD;
              return dsp::fft::fft2d(input, sign, nx, ny);
          }, py::arg("input"), py::arg("inverse") = false, py::arg("nx"), py::arg("ny"));

    // Expose FFT1D class
    py::class_<dsp::fft::FFT1D>(m, "FFT1D")
        .def(py::init<int>(), py::arg("size"))
        .def("execute", 
             [](dsp::fft::FFT1D& self, const cdouble_vec& input, bool inverse) {
                 int sign = inverse ? FFTW_BACKWARD : FFTW_FORWARD;
                 cdouble_vec output(input.size());
                 self.execute(input, output, sign);
                 return output;
             }, py::arg("input"), py::arg("inverse") = false);

    // Expose Polyphase class
    py::class_<dsp::polyphase::Polyphase>(m, "Polyphase")
        .def(py::init<int, int>(), py::arg("factor"), py::arg("num_taps"))
        .def(py::init<int, cdouble_vec>(), py::arg("factor"), py::arg("coeffs"))
        .def("branch", &dsp::polyphase::Polyphase::branch, py::arg("input"), py::arg("flip") = false)
        .def("interleave", &dsp::polyphase::Polyphase::interleave, py::arg("input"))
        .def("convolve_branches_decimate", &dsp::polyphase::Polyphase::convolve_branches_decimate, py::arg("branches"))
        .def("convolve_branches_interpolate", &dsp::polyphase::Polyphase::convolve_branches_interpolate, py::arg("branches"))
        .def("get_filter_slices", &dsp::polyphase::Polyphase::get_filter_slices)
        .def("sum_branches", &dsp::polyphase::Polyphase::sum_branches, py::arg("branches"))
        .def("interpolate", &dsp::polyphase::Polyphase::interpolate, py::arg("input"))
        .def("decimate", &dsp::polyphase::Polyphase::decimate, py::arg("input"));
}