#ifndef BOARD_H
#define BOARD_H

#include "BitBoard.h"
#include "Hash.h"

// Board state variables
extern U64 bitboards[12]; // piece bitboards
extern U64 occupancies[3]; // occupancy bitboards
extern int side; // side to move
extern int enpassant; // enpassant square
extern int castle; // castling rights
extern U64 hash_key; // "almost" unique position identifier aka hash key or position key
extern U64 repetition_table[1000]; // positions repetition table
extern int repetition_index; // repetition index
extern int ply; // half move counter

// Function prototypes
void print_board(void);
void parse_fen(char *fen);

#endif
