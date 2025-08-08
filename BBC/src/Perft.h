#ifndef PERFT_H
#define PERFT_H

#include "BitBoard.h"
#include "MoveGeneration.h"
#include "TimeControl.h"

// Global variables
extern U64 nodes;

// Function declarations
void perft_driver(int depth);
void perft_test(int depth);

#endif
