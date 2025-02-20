cmake_minimum_required(VERSION 3.16)
project(Signal_Sniper VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options to enable/disable components
option(ENABLE_TESTS "Enable GoogleTest Unit Tests" OFF)
option(ENABLE_PYBIND "Enable Pybind11 Python Bindings" OFF)
option(BUILD_RPM "Build an RPM package" OFF)
option(BUILD_DEB "Build a Debian package" OFF)

# Find Eigen3 (Header-only, so we only need include directory)
find_path(EIGEN3_INCLUDE_DIR Eigen/Core
    PATHS /usr/include/eigen3 /usr/local/include/eigen3
    PATH_SUFFIXES eigen3
)

if (EIGEN3_INCLUDE_DIR)
    message(STATUS "Found Eigen: ${EIGEN3_INCLUDE_DIR}")
    include_directories(${EIGEN3_INCLUDE_DIR})
else()
    message(FATAL_ERROR "Could not find Eigen headers")
endif()

# Find FFTW (Library and Headers)
find_library(FFTW_LIBRARY fftw3
    PATHS /usr/lib /usr/local/lib /usr/lib64 /usr/local/lib64
)

find_path(FFTW_INCLUDE_DIR fftw3.h
    PATHS /usr/include /usr/local/include
)

if (FFTW_LIBRARY AND FFTW_INCLUDE_DIR)
    message(STATUS "Found FFTW: ${FFTW_LIBRARY}")
    add_library(FFTW3::FFTW3 UNKNOWN IMPORTED)
    set_target_properties(FFTW3::FFTW3 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${FFTW_INCLUDE_DIR}"
        IMPORTED_LOCATION "${FFTW_LIBRARY}"
    )
else()
    message(FATAL_ERROR "Could not find FFTW library")
endif()

# Automatically find all source and header files
file(GLOB_RECURSE SIGNAL_SNIPER_HEADERS inc/**/*.h)
file(GLOB_RECURSE SIGNAL_SNIPER_SOURCES src/**/*.cc)

# Core Signal Sniper Library
add_library(signal_sniper SHARED ${SIGNAL_SNIPER_SOURCES})

# Include all header directories
target_include_directories(signal_sniper PUBLIC inc ${EIGEN3_INCLUDE_DIR})
target_link_libraries(signal_sniper PRIVATE FFTW3::FFTW3)

# Install library and headers
install(TARGETS signal_sniper DESTINATION lib)
install(DIRECTORY inc/ DESTINATION include)  # Install all headers while preserving structure

# GoogleTest (optional)
if(ENABLE_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    file(GLOB_RECURSE TEST_SOURCES tests/**/*.cc)
    add_executable(test_signal_sniper ${TEST_SOURCES})
    target_link_libraries(test_signal_sniper PRIVATE GTest::GTest GTest::Main signal_sniper)
    add_test(NAME SignalSniperTests COMMAND test_signal_sniper)
endif()

# Pybind11 Python Bindings (optional)
if(ENABLE_PYBIND)
    find_package(Python3 COMPONENTS Development REQUIRED)
    find_package(pybind11 REQUIRED)
    file(GLOB_RECURSE PYBIND_SOURCES python_bindings/**/*.cc)
    add_library(signal_sniper_python MODULE ${PYBIND_SOURCES})
    target_link_libraries(signal_sniper_python PRIVATE pybind11::module signal_sniper)
endif()

# RPM Packaging (optional)
if(BUILD_RPM)
    add_subdirectory(rpm)
endif()

# Debian Packaging (optional)
if(BUILD_DEB)
    add_subdirectory(deb)
endif()

# Adding cleaning target
# Add a custom clean target
add_custom_target(clean
    COMMAND ${CMAKE_COMMAND} --build . --target clean
    COMMAND rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake Makefile
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning all build files..."
)
