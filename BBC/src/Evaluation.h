#ifndef EVALUATION_H
#define EVALUATION_H

#include "BitBoard.h"
#include "MagicBitboards.h"
#include "Board.h"

// Material scores
extern int material_score[12];

// Positional score tables
extern const int pawn_score[64];
extern const int knight_score[64];
extern const int bishop_score[64];
extern const int rook_score[64];
extern const int king_score[64];

// Mirror score table for black pieces
extern const int mirror_score[128];

// Evaluation masks
extern U64 file_masks[64];
extern U64 rank_masks[64];
extern U64 isolated_masks[64];
extern U64 white_passed_masks[64];
extern U64 black_passed_masks[64];

// Rank extraction table
extern const int get_rank[64];

// Penalty and bonus constants
extern const int double_pawn_penalty;
extern const int isolated_pawn_penalty;
extern const int passed_pawn_bonus[8];
extern const int semi_open_file_score;
extern const int open_file_score;
extern const int king_shield_bonus;

// Function declarations
U64 set_file_rank_mask(int file_number, int rank_number);
void init_evaluation_masks();
int evaluate();

#endif
