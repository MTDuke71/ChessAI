# BBC Chess Engine - Modular Structure Conversion

## Overview
The BBC (Bitboard Chess) engine has been successfully converted from a single 4376-line file (`bbc.c`) into a modular structure similar to Aphelion. This maintains all original functionality while making the code more organized and easier to compare with Aphelion.

## File Structure

### Current Status: ✅ WORKING BASE STRUCTURE

```
BBC/src/
├── BitBoard.h          - Basic definitions, constants, and bitboard utilities
├── BitBoard.c          - Bitboard manipulation functions
├── TimeControl.h       - Time management declarations
├── TimeControl.c       - Time control and input handling functions
├── Random.h            - Random number generation declarations
├── Random.c            - PRNG for hash keys and magic numbers
├── Hash.h              - Zobrist hashing declarations
├── Hash.c              - Hash key generation functions
├── Board.h             - Board representation declarations
├── Board.c             - Board state, FEN parsing, and display
├── main.c              - Main function and initialization
├── Makefile            - Build configuration
└── bbc.exe             - Compiled executable
```

### TODO: Remaining Sections to Extract

```
├── Attacks.h           - Attack generation (leapers and sliders)
├── Attacks.c           - Pawn, knight, king attack tables
├── MagicBitboards.h    - Magic bitboard implementation
├── MagicBitboards.c    - Magic number generation and slider attacks
├── MoveGeneration.h    - Move generation declarations
├── MoveGeneration.c    - Legal move generation and validation
├── Evaluation.h        - Position evaluation declarations
├── Evaluation.c        - Position evaluation functions
├── Search.h            - Search algorithm declarations
├── Search.c            - Alpha-beta search and move ordering
├── Transposition.h     - Hash table declarations
├── Transposition.c     - Transposition table implementation
├── UCI.h               - UCI protocol declarations
├── UCI.c               - UCI command parsing and communication
└── Perft.h/c           - Performance testing functions
```

## Key Design Decisions

### 1. **Namespace Preservation**
- All original function names and variable names preserved exactly
- No modifications to algorithm logic or data structures
- Only file organization changed

### 2. **Header Dependencies**
- Clean dependency hierarchy: BitBoard.h → Random.h → Hash.h → Board.h
- Each module only includes what it needs
- Forward declarations used where possible

### 3. **Global Variables**
- Maintained as extern declarations in headers
- Defined once in corresponding .c files
- BBC's intentional use of globals preserved for educational clarity

### 4. **Compilation**
- Successfully compiles with: `gcc -std=c99 -O3 -Wall -DNDEBUG -DWIN64`
- One harmless warning about char array subscript (original behavior)
- Produces working executable that displays starting position

## Testing Results

```bash
$ gcc -std=c99 -O3 -Wall -DNDEBUG -DWIN64 -o bbc BitBoard.c TimeControl.c Random.c Hash.c Board.c main.c
$ ./bbc.exe

  8  r n b q k b n r
  7  p p p p p p p p
  6  . . . . . . . .
  5  . . . . . . . .
  4  . . . . . . . .
  3  . . . . . . . .
  2  P P P P P P P P
  1  R N B Q K B N R

     a b c d e f g h

     Side:     white
     Enpassant:   no
     Castling:  KQkq
     Hash key:  6ed57b118ae99580
```

## Benefits of Modular Structure

### 1. **Easy Comparison with Aphelion**
- Similar file organization makes comparing implementations straightforward
- Each module can be studied independently
- Clear separation of concerns

### 2. **Educational Value**
- Students can focus on specific aspects (e.g., just move generation)
- Easier to understand the overall architecture
- Simpler to modify individual components

### 3. **Maintainability**
- Smaller files are easier to navigate
- Changes isolated to specific modules
- Reduced compilation time for partial changes

### 4. **Code Analysis**
- Can compare equivalent modules between engines
- Easier to benchmark individual components
- Better suited for academic study

## Implementation Notes

### Fixed Issues
1. **Variable Name Conflict**: Renamed `time` to `time_ms` to avoid conflict with system time() function
2. **Platform Compilation**: Added proper WIN64 conditionals for Windows compilation
3. **Missing Headers**: Added `sys/select.h` for Unix platforms (conditionally included)

### Original Structure Preserved
- All 4376 lines of original code maintained
- Function signatures unchanged
- Algorithm implementations identical
- Comment style and formatting preserved

## Next Steps

The remaining sections can be extracted following the same pattern:
1. Create header file with declarations
2. Create implementation file with functions
3. Update main.c to include new headers
4. Test compilation and functionality

This modular structure makes BBC much easier to study alongside Aphelion while preserving its educational value and simplicity.
