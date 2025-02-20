#include <pybind11/pybind11.h>
#include "test_module.h"

namespace py = pybind11;

PYBIND11_MODULE(signal_sniper_python, m) {
    py::class_<TestModule>(m, "TestModule")
        .def(py::init<>())  // Constructor
        .def("add", &TestModule::add, "A function that adds two numbers");
}

