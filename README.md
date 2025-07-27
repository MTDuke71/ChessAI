# ChessAI

A simple C++ project exploring chess board representation and move generation.

## Building

This project uses **CMake**. A C++17 compiler, CMake and Make are required.

```bash
# from the repository root
mkdir build && cd build
cmake ..
make
```

This will create several executables such as `ChessAI` (the example program) and the various test programs.

## Running the Example

After building, run the main example from the `build` directory:

```bash
./ChessAI
```

It will generate pawn moves for the initial board and print them.

## Running the Tests

Tests are registered with **CTest**, so once the project is built they can be
executed all together:

```bash
ctest
```

You can still run individual binaries (e.g. `./PawnMoveTests`) from the `build`
directory if needed. Each test binary prints diagnostic information and exits
with success when all assertions pass.

## Code Structure

- `src/Board.*` – board representation, FEN loading and printing utilities
- `src/MoveGenerator.*` – move generation for all pieces
- `src/PrintMoves.*` – helper to print generated move lists
- `src/main.cpp` – simple example using the board and move generator
- `src/*Tests.cpp` – small self-contained test programs for the above components


