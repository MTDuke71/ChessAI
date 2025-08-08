#ifndef MOVEGENERATION_H
#define MOVEGENERATION_H

#include "BitBoard.h"
#include "Attacks.h"
#include "MagicBitboards.h"
#include "Board.h"
#include "Hash.h"

// Move encoding macros
#define encode_move(source, target, piece, promoted, capture, double, enpassant, castling) \
    (source) |          \
    (target << 6) |     \
    (piece << 12) |     \
    (promoted << 16) |  \
    (capture << 20) |   \
    (double << 21) |    \
    (enpassant << 22) | \
    (castling << 23)    

// Move extraction macros
#define get_move_source(move) (move & 0x3f)
#define get_move_target(move) ((move & 0xfc0) >> 6)
#define get_move_piece(move) ((move & 0xf000) >> 12)
#define get_move_promoted(move) ((move & 0xf0000) >> 16)
#define get_move_capture(move) (move & 0x100000)
#define get_move_double(move) (move & 0x200000)
#define get_move_enpassant(move) (move & 0x400000)
#define get_move_castling(move) (move & 0x800000)

// Board state preservation macros
#define copy_board()                                                      \
    U64 bitboards_copy[12], occupancies_copy[3];                          \
    int side_copy, enpassant_copy, castle_copy;                           \
    memcpy(bitboards_copy, bitboards, 96);                                \
    memcpy(occupancies_copy, occupancies, 24);                            \
    side_copy = side, enpassant_copy = enpassant, castle_copy = castle;   \
    U64 hash_key_copy = hash_key;                                         \

#define take_back()                                                       \
    memcpy(bitboards, bitboards_copy, 96);                                \
    memcpy(occupancies, occupancies_copy, 24);                            \
    side = side_copy, enpassant = enpassant_copy, castle = castle_copy;   \
    hash_key = hash_key_copy;                                             \

// Move types
enum { all_moves, only_captures };

// Move list structure
typedef struct {
    int moves[256];
    int count;
} moves;

// Castling rights update constants
extern const int castling_rights[64];

// Function declarations
int is_square_attacked(int square, int side);
void print_attacked_squares(int side);
void add_move(moves *move_list, int move);
void print_move(int move);
void print_move_list(moves *move_list);
int make_move(int move, int move_flag);
void generate_moves(moves *move_list);

#endif
