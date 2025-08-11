#pragma once
#include <cstdint>

// BBC-style direct bitboard manipulation for maximum performance
// This is designed to match BBC's ultra-efficient approach

typedef uint64_t U64;

// BBC-style piece encoding
enum { P, N, B, R, Q, K, p, n, b, r, q, k };

// Sides
enum { white, black, both };

// Castling rights (BBC-style)
enum { wk = 1, wq = 2, bk = 4, bq = 8 };

// BBC-style bit manipulation macros
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define pop_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))

// BBC-style engine core
class BBCStyleEngine {
public:
    // BBC-style bitboard array (direct access like BBC)
    U64 bitboards[12];
    
    // BBC-style occupancies
    U64 occupancies[3]; // [white, black, both]
    
    // Game state (BBC-style)
    int side;
    int enpassant;
    int castle;
    
    // Board copy stack for fast make/unmake (BBC-style)
    struct BoardState {
        U64 bitboards[12];
        U64 occupancies[3];
        int side;
        int enpassant; 
        int castle;
    };
    
    BoardState boardStack[256];  // Pre-allocated stack
    int stackIndex;
    
    BBCStyleEngine();
    void loadFromFEN(const char* fen);
    void copyBoard();     // BBC-style board copying
    void takeBack();      // BBC-style board restoration
    
    // BBC-style move representation (32-bit integer)
    struct Move {
        uint32_t data;
        
        Move() : data(0) {}  // Default constructor
        
        Move(int source, int target, int piece, int promoted = 0, 
             int capture = 0, int doublePush = 0, int enpass = 0, int castling = 0) {
            data = source | (target << 6) | (piece << 12) | (promoted << 16) |
                   (capture << 20) | (doublePush << 21) | (enpass << 22) | (castling << 23);
        }
        
        int source() const { return data & 0x3f; }
        int target() const { return (data >> 6) & 0x3f; }
        int piece() const { return (data >> 12) & 0xf; }
        int promoted() const { return (data >> 16) & 0xf; }
        int capture() const { return (data >> 20) & 1; }
        int doublePush() const { return (data >> 21) & 1; }
        int enpass() const { return (data >> 22) & 1; }
        int castling() const { return (data >> 23) & 1; }
    };
    
    struct MoveList {
        Move moves[256];
        int count;
        
        MoveList() : count(0) {}  // Default constructor
    };
    
    // BBC-style core functions
    void generateMoves(MoveList& moveList);
    int makeMove(const Move& move);     // Returns 1 if legal, 0 if illegal
    void updateOccupancies();          // BBC-style occupancy update
    bool isSquareAttacked(int square, int bySide);
    
    // BBC-style ultra-fast perft
    uint64_t perft(int depth);
    uint64_t perftDivide(int depth);
    
private:
    void initializeBitboards();
    uint64_t perftRecursive(int depth);
    
    // BBC-style attack checking
    bool isSquareAttackedByPawn(int square, int bySide);
    bool isSquareAttackedByKnight(int square, int bySide);
    bool isSquareAttackedByBishop(int square, int bySide);
    bool isSquareAttackedByRook(int square, int bySide);
    bool isSquareAttackedByQueen(int square, int bySide);
    bool isSquareAttackedByKing(int square, int bySide);
};
