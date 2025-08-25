#!/bin/bash
# Script to run tests with proper library path to avoid miniconda conflicts

export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH

# Build the project
cd build && make -j4

# Run all tests
echo "Running all tests..."
ctest --output-on-failure

# Run just the Google Test example
echo "Running Google Test example..."
ctest -R gtest_example --output-on-failure