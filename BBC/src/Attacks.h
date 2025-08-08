#ifndef ATTACKS_H
#define ATTACKS_H

#include "BitBoard.h"

// File mask constants
extern const U64 not_a_file;
extern const U64 not_h_file;
extern const U64 not_hg_file;
extern const U64 not_ab_file;

// Relevant occupancy bit counts for magic bitboards
extern const int bishop_relevant_bits[64];
extern const int rook_relevant_bits[64];

// Magic numbers for slider attacks
extern U64 rook_magic_numbers[64];
extern U64 bishop_magic_numbers[64];

// Attack tables
extern U64 pawn_attacks[2][64];
extern U64 knight_attacks[64];
extern U64 king_attacks[64];
extern U64 bishop_masks[64];
extern U64 rook_masks[64];
extern U64 bishop_attacks[64][512];
extern U64 rook_attacks[64][4096];

// Function declarations
U64 mask_pawn_attacks(int side, int square);
U64 mask_knight_attacks(int square);
U64 mask_king_attacks(int square);
U64 mask_bishop_attacks(int square);
U64 mask_rook_attacks(int square);
U64 bishop_attacks_on_the_fly(int square, U64 block);
U64 rook_attacks_on_the_fly(int square, U64 block);
void init_leapers_attacks();
U64 set_occupancy(int index, int bits_in_mask, U64 attack_mask);

#endif
