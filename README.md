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
- `src/Perft.*` – helper function to run perft node counts

## Usage Examples

Several small programs in the `examples` directory demonstrate how to work with
the API:

- `create_position.cpp` – manually set up a board and print it.
- `generate_moves.cpp` – load a FEN string and list all legal moves.
- `search_best_move.cpp` – use the engine to pick a move from a position.
- `perft.cpp` – calculate move counts at a given depth.
- `UCI` – command line interface implementing the UCI protocol.

After building, run them from the `build` directory just like the main example:

```bash
./CreatePosition
./GenerateMoves
./SearchBestMove
./Perft <FEN> <depth>
./UCI
```


