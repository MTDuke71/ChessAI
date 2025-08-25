# Google Test Integration

This project now supports Google Test framework for unit testing alongside the existing custom testing approach.

## Setup

Google Test is automatically detected and configured by the CMake build system. If Google Test is found on your system, additional test targets will be built.

## Building and Running Tests

### Build all tests including Google Test:
```bash
mkdir -p build
cd build
cmake ..
make -j4
```

### Run Google Test examples:
```bash
# Run all Google Test targets
export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH
ctest -R "gtest" --output-on-failure

# Or run individual test executables
./gtest_example
./BoardTestGtest
```

### Use the convenience script:
```bash
./run_tests.sh
```

## Google Test Examples

1. **gtest_example.cpp** - Basic demonstration of Google Test framework integration with chess engine components
2. **BoardTestGtest.cpp** - Conversion of the original BoardTest.cpp to use Google Test assertions and structure

## Features Demonstrated

- Basic Google Test assertions (EXPECT_EQ, EXPECT_TRUE, EXPECT_FALSE)
- Test fixtures and suites
- Integration with existing chess engine classes (Board, MoveGenerator)
- Conversion of existing custom tests to Google Test format
- CTest integration for running Google Test targets

## Benefits of Google Test

- Better error reporting and test output formatting
- Parameterized tests capability
- Test filtering and selection
- Detailed failure information
- Integration with continuous integration systems
- Standard testing framework familiar to C++ developers

## Existing Tests

The original custom tests using assert() statements continue to work and are still available. Google Test integration is additive and doesn't replace the existing testing infrastructure.