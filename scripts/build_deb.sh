#!/bin/bash

set -e  # Exit on error

echo "📦 Building Debian Package..."

# Create build directory if it doesn't exist
mkdir -p build && cd build

# Run CMake with Debian packaging enabled
cmake .. -DBUILD_DEB=ON

# Compile and generate DEB package
make -j$(nproc)
make package

echo "✅ Debian Package Built Successfully."

