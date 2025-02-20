#!/bin/bash

set -e  # Exit on error

echo "📦 Building RPM Package..."

# Create build directory if it doesn't exist
mkdir -p build && cd build

# Run CMake with RPM packaging enabled
cmake .. -DBUILD_RPM=ON

# Compile and generate RPM
make -j$(nproc)
make package

echo "✅ RPM Package Built Successfully."

