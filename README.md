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

The build produces a test executable for each source file ending with `*Tests.cpp` and a `BoardTest`.
Run them individually inside the `build` directory:

```bash
./BoardTest
./PawnMoveTests
./KnightMoveTests
./SliderMoveTests
./KingMoveTests
```

Each test binary prints diagnostic information and ends with a success message when all assertions pass.

## Code Structure

- `src/Board.*` – board representation, FEN loading and printing utilities
- `src/MoveGenerator.*` – move generation for all pieces
- `src/PrintMoves.*` – helper to print generated move lists
- `src/main.cpp` – simple example using the board and move generator
- `src/*Tests.cpp` – small self-contained test programs for the above components


