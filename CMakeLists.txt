cmake_minimum_required(VERSION 3.16)
project(Signal_Sniper VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SUPPRESS_REGENERATION OFF)

# Options to enable/disable components
option(ENABLE_TESTS "Enable GoogleTest Unit Tests" OFF)
option(ENABLE_PYBIND "Enable Pybind11 Python Bindings" OFF)
option(BUILD_RPM "Build an RPM package" OFF)
option(BUILD_DEB "Build a Debian package" OFF)
option(INSTALL_TO_SYSTEM "Install to the local system" OFF)

# Optimization and Warning Options
option(ENABLE_OPTIMIZED_BUILD "Enable optimized (-O2) compilation" OFF)
option(ENABLE_WARNINGS "Enable strict warnings (-Wall -Wextra)" OFF)

if (ENABLE_WARNINGS)
    message(STATUS "Enabling warnings for all compilation: -Wall -Wextra")
    add_compile_options(signal_sniper PRIVATE -Wall -Wextra)
endif()

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
file(GLOB_RECURSE SIGNAL_SNIPER_SOURCES LIST_DIRECTORIES false RELATIVE ${CMAKE_SOURCE_DIR} CONFIGURE_DEPENDS src/*.cc)

# Debugging output: Show all detected sources
message(STATUS "Discovered source files: ${SIGNAL_SNIPER_SOURCES}")

# Core Signal Sniper Library
add_library(signal_sniper SHARED ${SIGNAL_SNIPER_SOURCES})
target_compile_options(signal_sniper PRIVATE -mavx2)

if (ENABLE_OPTIMIZED_BUILD)
    message(STATUS "Enabling optimized build for signal_sniper: -O2")
    target_compile_options(signal_sniper PRIVATE -O2)
endif()

# Include all header directories
target_include_directories(signal_sniper PUBLIC inc ${EIGEN3_INCLUDE_DIR})
target_link_libraries(signal_sniper PRIVATE FFTW3::FFTW3)

# Install library and headers
if (INSTALL_TO_SYSTEM)
    install(TARGETS signal_sniper DESTINATION lib)
    install(DIRECTORY inc/ DESTINATION include)  # Install all headers while preserving structure
endif()

# GoogleTest (optional)
if(ENABLE_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    
    file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false RELATIVE ${CMAKE_SOURCE_DIR} CONFIGURE_DEPENDS tests/*.cc)
    message(STATUS "Discovered test source files: ${TEST_SOURCES}")

    if(TEST_SOURCES)
        add_executable(test_signal_sniper ${TEST_SOURCES})
        target_link_libraries(test_signal_sniper PRIVATE GTest::GTest GTest::Main signal_sniper)
        add_test(NAME SignalSniperTests COMMAND test_signal_sniper)
    else()
        message(WARNING "No test files found in tests/ directory. Skipping test target.")
    endif()
endif()

# Pybind11 Python Bindings (optional)
if(ENABLE_PYBIND)
    # Check if a virtual environment exists and use it if available
    if(EXISTS "${CMAKE_SOURCE_DIR}/.venv/bin/python3")
        set(Python3_EXECUTABLE "${CMAKE_SOURCE_DIR}/.venv/bin/python3")
        message(STATUS "Using virtual environment Python: ${Python3_EXECUTABLE}")
    endif()
    
    # Run find package which will set python3_SITELIB
    find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
    find_package(pybind11 REQUIRED)
    
    # Ensure Python3_SITELIB is set
    if(NOT Python3_SITELIB)
        message(FATAL_ERROR "CMake did not detect Python3_SITELIB. Ensure Python3 is properly found.")
    endif()

    file(GLOB_RECURSE PYBIND_SOURCES LIST_DIRECTORIES false RELATIVE ${CMAKE_SOURCE_DIR} CONFIGURE_DEPENDS pybind_src/*.cc)

    if(PYBIND_SOURCES)
        message(STATUS "Discovered pybind source files: ${PYBIND_SOURCES}")

        # Add the poybind module and link against the other built library
        pybind11_add_module(signal_sniper_python ${PYBIND_SOURCES})

        if (ENABLE_OPTIMIZED_BUILD)
            message(STATUS "Enabling optimized build for signal_sniper: -O2")
            target_compile_options(signal_sniper PRIVATE -O2)
        endif()

        # Set RPATH so that signal_sniper_python.so can find libsignal_sniper.so
        set_target_properties(signal_sniper_python PROPERTIES INSTALL_RPATH "\$ORIGIN")
        target_link_libraries(signal_sniper_python PRIVATE signal_sniper)

        message("Installing pybind modules at: ${Python3_SITELIB}")
        install(TARGETS signal_sniper DESTINATION ${Python3_SITELIB})
        install(TARGETS signal_sniper_python DESTINATION ${Python3_SITELIB})
    else()
        message(WARNING "No test files found in tests/ directory. Skipping test target.")
    endif()
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
add_custom_target(cln
    COMMAND ${CMAKE_COMMAND} --build . --target clean
    COMMAND rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake Makefile
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning all build files..."
)

