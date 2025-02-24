# Signal Sniper

Signal Sniper is an open-source DSP library designed for various signal and communications applications. It provides efficient implementations of common DSP operations using Eigen and FFTW3.

## Features

- RPM & DEB package build and installation
- Python bindings using Pybind11
- Unit testing with GoogleTest
- DSP functionality like overlap save convolution, sinc filtering, Polyphase Channelizing

### Proposed features
- FEC like LDPC and Polar encoding/decoding
- Modulation/Demodulation for different QAM and PSK schemes

## Dependencies

Before building the project, ensure that the following dependencies are installed:

### Required dependencies
- **CMake**: Build system
- **Eigen3**: Linear algebra library
- **FFTW3**: Fast Fourier Transform library

### Development Dependencies
- **GoogleTest**: Unit testing framework
- **Pybind11**: C++ bindings for Python
- **python3**: up-to-date python for venv where packages in python/requirements.txt will be used

For detailed installation instructions, refer to the [DEPENDENCIES.md](DEPENDENCIES.md) file.

## Building and Installing

### Using Setup Scripts

1. **Setup Debug Environment**:
   Run the `setup_debug_env.sh` script to create a virtual environment, install Python dependencies, and build the project with testing and Pybind11 enabled.
   ```
   ./scripts/setup_debug_env.sh
   ```

2. **Source the Environment**:
   Source the `env.sh` script to expose the `mkf` and `mk_clean` aliases for building and cleaning the project.
   ```
   source scripts/env.sh
   ```

3. **Build and Install**:
   Use the `mkf` alias to build and install the project.
   ```
   mkf
   ```

4. **Clean the Build**:
   Use the `mk_clean` alias to clean the build and remove the virtual environment.
   ```
   mk_clean
   ```

### Manual CMake Build

If you prefer to run CMake manually, you can use the following flags to customize the build:

- `ENABLE_TESTS`: Enable GoogleTest unit tests (default: OFF)
- `ENABLE_PYBIND`: Enable Pybind11 Python bindings (default: OFF)
- `BUILD_RPM`: Build an RPM package (default: OFF)
- `BUILD_DEB`: Build a Debian package (default: OFF)
- `INSTALL_TO_SYSTEM`: Install to the local system (default: OFF)

#### Example: Build with Tests and Python Bindings

1. **Create Build Directory**:
   ```
   mkdir -p build && cd build
   ```

2. **Run CMake**:
   ```
   cmake .. -DENABLE_TESTS=ON -DENABLE_PYBIND=ON
   ```

3. **Build the Project**:
   ```
   make -j$(nproc)
   ```

4. **Install the Project**:
   ```
   make install
   ```

### Building Packages

#### Build RPM Package

1. **Run the RPM Build Script**:
   ```
   ./scripts/build_rpm.sh
   ```

#### Build Debian Package

1. **Run the Debian Build Script**:
   ```
   ./scripts/build_deb.sh
   ```

## Running Tests

If you have enabled tests during the build, you can run the gtests from the project root using :

```
./test_signal_sniper
```
You can filter particular tests like so:

for test:
```cpp
TEST(MY_TEST_MODULE, my_test_name) {
... test_content ...
}

```
by passing this command line arg:
```sh
./test_signal_sniper --gtest_filter=MY_TEST_MODULE.my_test_name
```

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request on GitHub.

## Contact

For questions or support, please contact [ajh](mailto:ajh_horton@hotmail.com).
