cmake_minimum_required(VERSION 3.16)
project(DSP_Library VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options to enable/disable components
option(ENABLE_TESTS "Enable GoogleTest Unit Tests" OFF)
option(ENABLE_PYBIND "Enable Pybind11 Python Bindings" OFF)
option(BUILD_RPM "Build an RPM package" OFF)

# Dependencies
find_package(Eigen3 REQUIRED)
find_package(FFTW3 REQUIRED)

# Core DSP Library
add_library(dsp_lib SHARED
    src/dsp.cpp
    src/filters.cpp
)
target_include_directories(dsp_lib PUBLIC ${EIGEN3_INCLUDE_DIR})
target_link_libraries(dsp_lib PRIVATE fftw3)

install(TARGETS dsp_lib DESTINATION lib)
install(FILES src/dsp.hpp src/filters.hpp DESTINATION include)

# GoogleTest (optional)
if(ENABLE_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    add_executable(test_dsp tests/dsp_test.cpp tests/filters_test.cpp)
    target_link_libraries(test_dsp PRIVATE GTest::GTest GTest::Main dsp_lib)
    add_test(NAME DSPTests COMMAND test_dsp)
endif()

# Pybind11 Python Bindings (optional)
if(ENABLE_PYBIND)
    find_package(Python3 COMPONENTS Development REQUIRED)
    find_package(pybind11 REQUIRED)
    add_library(dsp_python MODULE python_bindings/module.cpp)
    target_link_libraries(dsp_python PRIVATE pybind11::module dsp_lib)
endif()

# RPM Packaging (optional)
if(BUILD_RPM)
    add_subdirectory(rpm)

if(BUILD_DEB)
    add_subdirectory(DEB)
endif()

