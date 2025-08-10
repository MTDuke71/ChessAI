#ifndef BITBOARD_H
#define BITBOARD_H

// system headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef WIN64
    #include <windows.h>
#else
    #include <sys/time.h>
    #include <sys/select.h>
#endif

// define bitboard data type
#define U64 unsigned long long

// FEN dedug positions
#define empty_board "8/8/8/8/8/8/8/8 b - - "
#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define cmk_position "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "
#define repetitions "2r3k1/R7/8/1R6/8/8/P4KPP/8 w - - 0 40 "

// board squares
enum {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1, no_sq
};

// encode pieces
enum { P, N, B, R, Q, K, p, n, b, r, q, k };

// sides to move (colors)
enum { white, black, both };

// bishop and rook
enum { rook, bishop };

// castling rights binary encoding
/*

    bin  dec
    
   0001    1  white king can castle to the king side
   0010    2  white king can castle to the queen side
   0100    4  black king can castle to the king side
   1000    8  black king can castle to the queen side

   examples

   1111       both sides an castle both directions
   1001       black king => queen side
              white king => king side

*/
enum { wk = 1, wq = 2, bk = 4, bq = 8 };

// convert squares to coordinates
extern const char *square_to_coordinates[];

// ASCII pieces
extern char ascii_pieces[12];

// unicode pieces
extern char *unicode_pieces[12];

// convert ASCII character pieces to encoded constants
extern int char_pieces[];

// promoted pieces
extern char promoted_pieces[];

// Bitboard manipulation macros
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define pop_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))

// Bit counting
static inline int count_bits(U64 bitboard) {
    // bit counter
    int count = 0;
    
    // consecutively reset LSB
    while (bitboard)
    {
        // increment count
        count++;
        
        // reset LSB
        bitboard &= bitboard - 1;
    }
    
    // return bit count
    return count;
}

// Get LSB index
static inline int get_ls1b_index(U64 bitboard) {
    // make sure bitboard is not 0
    if (bitboard)
    {
        // count trailing bits before LSB
        return count_bits((bitboard & -bitboard) - 1);
    }
    
    // otherwise
    else
        // return illegal index
        return -1;
}

// Utility functions
void print_bitboard(U64 bitboard);

#endif
