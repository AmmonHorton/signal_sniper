# 📦 DSP Library Dependencies

This document lists the dependencies required to build and run the DSP Library.

## 🛠️ Build Dependencies

### 1️⃣ **Required C++ Libraries**
| Dependency  | Purpose |
|------------|---------|
| **Eigen3** | Linear algebra (used for signal processing) |
| **FFTW3**  | Fast Fourier Transform library |
| **GoogleTest** | Unit testing framework |
| **Pybind11** | Python bindings for C++ |

### 2️⃣ **Python Dependencies**
| Dependency  | Purpose |
|------------|---------|
| **NumPy**  | Numerical computations |
| **SciPy**  | Scientific computing and signal processing |
| **Pybind11** | Required for Python bindings |
| **pytest** | Used for testing the Python API |

---

## 🏗️ Installation Instructions

### **🖥️ Ubuntu (WSL)**
Run the following commands to install dependencies:
```sh
sudo apt update && sudo apt install -y \
    build-essential cmake python3-dev \
    libeigen3-dev libfftw3-dev libgtest-dev \
    python3-pip python3-venv

pip install -r python/requirements.txt

