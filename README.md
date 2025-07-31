# Aphelion

A simple C++ project exploring chess board representation and move generation.

## Building

This project uses **CMake**. A C++17 compiler, CMake and Make are required.

```bash
# from the repository root
mkdir build && cd build
cmake ..
make
```

This will create several executables such as `Example` (a demo program) and `Aphelion` (the UCI engine) along with the various test programs.

## Running the Example

After building, run the main example from the `build` directory:

```bash
./Example
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
- `src/Example.cpp` – simple example using the board and move generator
- `test/*.cpp` – small self-contained test programs for the above components
- `src/Perft.*` – helper function to run perft node counts

## Usage Examples

Several small programs in the `examples` directory demonstrate how to work with
the API:

- `create_position.cpp` – manually set up a board and print it.
- `generate_moves.cpp` – load a FEN string and list all legal moves.
- `search_best_move.cpp` – use the engine to pick a move from a position.
- `perft.cpp` – calculate move counts at a given depth.
- `Aphelion` – command line interface implementing the UCI protocol.

After building, run them from the `build` directory just like the main example:

```bash
./CreatePosition
./GenerateMoves
./SearchBestMove
./Perft <FEN> <depth>
./Aphelion
```

## Implemented Features

- Board representation with FEN parsing/printing and comprehensive move generation.
- Perft utilities and unit tests for move generation correctness.
- Evaluation function using piece-square tables, mobility and development bonuses, with game-phase awareness.
- Minimax search with alpha-beta pruning, principal variation search and basic time management.
- Transposition tables with Zobrist hashing for reusing evaluated positions.
- Command-line UCI engine along with example programs for move generation and search.
- Quiescence search to reduce horizon effects in tactical positions.
- Opening book integration for common starting positions.
- Endgame tablebase lookup for perfect play in simplified endings.
- Parallel root search to utilize multiple CPU cores during move calculation.
- Move ordering using capture heuristics for faster alpha-beta pruning.
- Iterative deepening for improved move ordering and controllable search time.
- Supports `go infinite` and `stop` commands for indefinite search sessions.


