#ifndef HASH_H
#define HASH_H

#include "BitBoard.h"
#include "Random.h"

// Zobrist keys
extern U64 piece_keys[12][64]; // random piece keys [piece][square]
extern U64 enpassant_keys[64]; // random enpassant keys [square]
extern U64 castle_keys[16]; // random castling keys
extern U64 side_key; // random side key

// Function prototypes
void init_random_keys(void);
U64 generate_hash_key(void);

#endif
