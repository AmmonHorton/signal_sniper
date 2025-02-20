#!/bin/bash

set -e  # Exit on error

echo "🛠️ Setting up Debug Environment..."

# Create virtual environment to link pybind against
if [ ! -d ".venv" ]; then
    echo "🐍 Creating Python Virtual Environment..."
    python3 -m venv .venv
fi

# Activate virtual environment
source .venv/bin/activate

# Install Python dependencies
if [ -f "python/requirements.txt" ]; then
    echo "📦 Installing Python dependencies..."
    pip install --upgrade pip
    pip install -r python/requirements.txt
else
    echo "⚠️ No requirements.txt found in python/ directory!"
fi

# Create build directory if it doesn't exist
mkdir -p build && cd build

# Run CMake with testing and Pybind enabled
cmake .. -DENABLE_TESTS=ON -DENABLE_PYBIND=ON

# Compile everything
make

# Install the compiled .so library into the venv site-packages folder
make install

echo "✅ Debug Build Complete."

# Move back to root
cd ..


echo "✅ Debug Environment Ready. Activate it using: source ./scripts/env.sh"

