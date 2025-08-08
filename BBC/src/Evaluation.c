#include "Evaluation.h"

// material score
int material_score[12] = {
    100,      // white pawn score
    300,      // white knight scrore
    350,      // white bishop score
    500,      // white rook score
   1000,      // white queen score
  10000,      // white king score
   -100,      // black pawn score
   -300,      // black knight scrore
   -350,      // black bishop score
   -500,      // black rook score
  -1000,      // black queen score
 -10000,      // black king score
};

// pawn positional score
const int pawn_score[64] = 
{
    90,  90,  90,  90,  90,  90,  90,  90,
    30,  30,  30,  40,  40,  30,  30,  30,
    20,  20,  20,  30,  30,  30,  20,  20,
    10,  10,  10,  20,  20,  10,  10,  10,
     5,   5,  10,  20,  20,   5,   5,   5,
     0,   0,   0,   5,   5,   0,   0,   0,
     0,   0,   0, -10, -10,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0
};

// knight positional score
const int knight_score[64] = 
{
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,  10,  10,   0,   0,  -5,
    -5,   5,  20,  20,  20,  20,   5,  -5,
    -5,  10,  20,  30,  30,  20,  10,  -5,
    -5,  10,  20,  30,  30,  20,  10,  -5,
    -5,   5,  20,  10,  10,  20,   5,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5, -10,   0,   0,   0,   0, -10,  -5
};

// bishop positional score
const int bishop_score[64] = 
{
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,  20,   0,  10,  10,   0,  20,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,  10,   0,   0,   0,   0,  10,   0,
     0,  30,   0,   0,   0,   0,  30,   0,
     0,   0, -10,   0,   0, -10,   0,   0
};

// rook positional score
const int rook_score[64] =
{
    50,  50,  50,  50,  50,  50,  50,  50,
    50,  50,  50,  50,  50,  50,  50,  50,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,   0,  20,  20,   0,   0,   0

};

// king positional score
const int king_score[64] = 
{
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   5,   5,   5,   5,   0,   0,
     0,   5,   5,  10,  10,   5,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   0,   5,  10,  10,   5,   0,   0,
     0,   5,   5,  -5,  -5,   0,   5,   0,
     0,   0,   5,   0, -15,   0,  10,   0
};

// mirror positional score tables for opposite side
const int mirror_score[128] =
{
	a1, b1, c1, d1, e1, f1, g1, h1,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a8, b8, c8, d8, e8, f8, g8, h8
};

// file masks [square]
U64 file_masks[64];

// rank masks [square]
U64 rank_masks[64];

// isolated pawn masks [square]
U64 isolated_masks[64];

// white passed pawn masks [square]
U64 white_passed_masks[64];

// black passed pawn masks [square]
U64 black_passed_masks[64];

// extract rank from a square [square]
const int get_rank[64] =
{
    7, 7, 7, 7, 7, 7, 7, 7,
    6, 6, 6, 6, 6, 6, 6, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    4, 4, 4, 4, 4, 4, 4, 4,
    3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0
};

// double pawns penalty
const int double_pawn_penalty = -10;

// isolated pawn penalty
const int isolated_pawn_penalty = -10;

// passed pawn bonus
const int passed_pawn_bonus[8] = { 0, 10, 30, 50, 75, 100, 150, 200 }; 

// semi open file score
const int semi_open_file_score = 10;

// open file score
const int open_file_score = 15;

// king's shield bonus
const int king_shield_bonus = 5;

// set file or rank mask
U64 set_file_rank_mask(int file_number, int rank_number)
{
    // file or rank mask
    U64 mask = 0ULL;
    
    // loop over ranks
    for (int rank = 0; rank < 8; rank++)
    {
        // loop over files
        for (int file = 0; file < 8; file++)
        {
            // init square
            int square = rank * 8 + file;
            
            if (file_number != -1)
            {
                // on file match
                if (file == file_number)
                    // set bit on mask
                    mask |= set_bit(mask, square);
            }
            
            else if (rank_number != -1)
            {
                // on rank match
                if (rank == rank_number)
                    // set bit on mask
                    mask |= set_bit(mask, square);
            }
        }
    }
    
    // return mask
    return mask;
}

// init evaluation masks
void init_evaluation_masks()
{
    /******** Init file masks ********/
    
    // loop over ranks
    for (int rank = 0; rank < 8; rank++)
    {
        // loop over files
        for (int file = 0; file < 8; file++)
        {
            // init square
            int square = rank * 8 + file;
            
            // init file mask for a current square
            file_masks[square] |= set_file_rank_mask(file, -1);
        }
    }
    
    /******** Init rank masks ********/
    
    // loop over ranks
    for (int rank = 0; rank < 8; rank++)
    {
        // loop over files
        for (int file = 0; file < 8; file++)
        {
            // init square
            int square = rank * 8 + file;
            
            // init rank mask for a current square
            rank_masks[square] |= set_file_rank_mask(-1, rank);
        }
    }
    
    /******** Init isolated masks ********/
    
    // loop over ranks
    for (int rank = 0; rank < 8; rank++)
    {
        // loop over files
        for (int file = 0; file < 8; file++)
        {
            // init square
            int square = rank * 8 + file;
            
            // init isolated pawns masks for a current square
            isolated_masks[square] |= set_file_rank_mask(file - 1, -1);
            isolated_masks[square] |= set_file_rank_mask(file + 1, -1);
        }
    }
    
    /******** White passed masks ********/
    
    // loop over ranks
    for (int rank = 0; rank < 8; rank++)
    {
        // loop over files
        for (int file = 0; file < 8; file++)
        {
            // init square
            int square = rank * 8 + file;
            
            // init white passed pawns mask for a current square
            white_passed_masks[square] |= set_file_rank_mask(file - 1, -1);
            white_passed_masks[square] |= set_file_rank_mask(file, -1);
            white_passed_masks[square] |= set_file_rank_mask(file + 1, -1);
            
            // loop over redudant ranks
            for (int i = 0; i < (8 - rank); i++)
                // reset redudant bits 
                white_passed_masks[square] &= ~rank_masks[(7 - i) * 8 + file];
        }
    }
    
    /******** Black passed masks ********/
    
    // loop over ranks
    for (int rank = 0; rank < 8; rank++)
    {
        // loop over files
        for (int file = 0; file < 8; file++)
        {
            // init square
            int square = rank * 8 + file;
            
            // init black passed pawns mask for a current square
            black_passed_masks[square] |= set_file_rank_mask(file - 1, -1);
            black_passed_masks[square] |= set_file_rank_mask(file, -1);
            black_passed_masks[square] |= set_file_rank_mask(file + 1, -1);
            
            // loop over redudant ranks
            for (int i = 0; i < rank + 1; i++)
                // reset redudant bits 
                black_passed_masks[square] &= ~rank_masks[i * 8 + file];            
        }
    }
}

// position evaluation
int evaluate()
{
    // static evaluation score
    int score = 0;
    
    // current pieces bitboard copy
    U64 bitboard;
    
    // init piece & square
    int piece, square;
    
    // penalties
    int double_pawns = 0;
    
    // loop over piece bitboards
    for (int bb_piece = P; bb_piece <= k; bb_piece++)
    {
        // init piece bitboard copy
        bitboard = bitboards[bb_piece];
        
        // loop over pieces within a bitboard
        while (bitboard)
        {
            // init piece
            piece = bb_piece;
            
            // init square
            square = get_ls1b_index(bitboard);
            
            // score material weights
            score += material_score[piece];
            
            // score positional piece scores
            switch (piece)
            {
                // evaluate white pawns
                case P:
                    // positional score
                    score += pawn_score[square];
                    
                    // double pawn penalty
                    double_pawns = count_bits(bitboards[P] & file_masks[square]);
                    
                    // on double pawns (tripple, etc)
                    if (double_pawns > 1)
                        score += double_pawns * double_pawn_penalty;
                    
                    // on isolated pawn
                    if ((bitboards[P] & isolated_masks[square]) == 0)
                        // give an isolated pawn penalty
                        score += isolated_pawn_penalty;
                    
                    // on passed pawn
                    if ((white_passed_masks[square] & bitboards[p]) == 0)
                        // give passed pawn bonus
                        score += passed_pawn_bonus[get_rank[square]];

                    break;
                
                // evaluate white knights
                case N:
                    // positional score
                    score += knight_score[square];
                    break;
                
                // evaluate white bishops
                case B:
                    // positional scores
                    score += bishop_score[square];
                    
                    // mobility
                    score += count_bits(get_bishop_attacks(square, occupancies[both]));
                    
                    break;
                
                // evaluate white rooks
                case R:
                    // positional score
                    score += rook_score[square];
                    
                    // semi open file
                    if ((bitboards[P] & file_masks[square]) == 0)
                        // add semi open file bonus
                        score += semi_open_file_score;
                    
                    // semi open file
                    if (((bitboards[P] | bitboards[p]) & file_masks[square]) == 0)
                        // add semi open file bonus
                        score += open_file_score;
                    
                    break;
                
                // evaluate white queens
                case Q:
                    // mobility
                    score += count_bits(get_queen_attacks(square, occupancies[both]));
                    break;
                
                // evaluate white king
                case K:
                    // posirional score
                    score += king_score[square];
                    
                    // semi open file
                    if ((bitboards[P] & file_masks[square]) == 0)
                        // add semi open file penalty
                        score -= semi_open_file_score;
                    
                    // semi open file
                    if (((bitboards[P] | bitboards[p]) & file_masks[square]) == 0)
                        // add semi open file penalty
                        score -= open_file_score;
                    
                    // king safety bonus
                    score += count_bits(king_attacks[square] & occupancies[white]) * king_shield_bonus;
                    break;

                // evaluate black pawns
                case p:
                    // positional score
                    score -= pawn_score[mirror_score[square]];

                    // double pawn penalty
                    double_pawns = count_bits(bitboards[p] & file_masks[square]);
                    
                    // on double pawns (tripple, etc)
                    if (double_pawns > 1)
                        score -= double_pawns * double_pawn_penalty;
                    
                    // on isolated pawnd
                    if ((bitboards[p] & isolated_masks[square]) == 0)
                        // give an isolated pawn penalty
                        score -= isolated_pawn_penalty;
                    
                    // on passed pawn
                    if ((black_passed_masks[square] & bitboards[P]) == 0)
                        // give passed pawn bonus
                        score -= passed_pawn_bonus[get_rank[mirror_score[square]]];

                    break;
                
                // evaluate black knights
                case n:
                    // positional score
                    score -= knight_score[mirror_score[square]];
                    break;
                
                // evaluate black bishops
                case b:
                    // positional score
                    score -= bishop_score[mirror_score[square]];
                    
                    // mobility
                    score -= count_bits(get_bishop_attacks(square, occupancies[both]));
                    break;
                
                // evaluate black rooks
                case r:
                    // positional score
                    score -= rook_score[mirror_score[square]];
                    
                    // semi open file
                    if ((bitboards[p] & file_masks[square]) == 0)
                        // add semi open file bonus
                        score -= semi_open_file_score;
                    
                    // semi open file
                    if (((bitboards[P] | bitboards[p]) & file_masks[square]) == 0)
                        // add semi open file bonus
                        score -= open_file_score;
                    
                    break;
                
                // evaluate black queens
                case q:
                    // mobility
                    score -= count_bits(get_queen_attacks(square, occupancies[both]));
                    break;
                
                // evaluate black king
                case k:
                    // positional score
                    score -= king_score[mirror_score[square]];
                    
                    // semi open file
                    if ((bitboards[p] & file_masks[square]) == 0)
                        // add semi open file penalty
                        score += semi_open_file_score;
                    
                    // semi open file
                    if (((bitboards[P] | bitboards[p]) & file_masks[square]) == 0)
                        // add semi open file penalty
                        score += open_file_score;
                    
                    // king safety bonus
                    score -= count_bits(king_attacks[square] & occupancies[black]) * king_shield_bonus;
                    break;
            }

            // pop ls1b
            pop_bit(bitboard, square);
        }
    }
    
    // return final evaluation based on side
    return (side == white) ? score : -score;
}
