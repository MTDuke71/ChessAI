#pragma once
#include "Board.h"
#include <vector>
#include <cstdint>

class FastMoveGenerator {
public:
    // Use integer move encoding like BBC instead of strings
    struct Move {
        uint32_t data; // Changed from uint16_t to uint32_t to accommodate all fields
        
        Move() : data(0) {} // Default constructor
        
        Move(int from, int to, int piece = 0, int promotion = 0, 
             bool capture = false, bool doublePawn = false, 
             bool enPassant = false, bool castling = false) {
            data = from | (to << 6) | (piece << 12) | (promotion << 16) |
                   (capture << 20) | (doublePawn << 21) | 
                   (enPassant << 22) | (castling << 23);
        }
        
        int from() const { return data & 0x3f; }
        int to() const { return (data >> 6) & 0x3f; }
        int piece() const { return (data >> 12) & 0xf; }
        int promotion() const { return (data >> 16) & 0xf; }
        bool isCapture() const { return data & 0x100000; }
        bool isDoublePawn() const { return data & 0x200000; }
        bool isEnPassant() const { return data & 0x400000; }
        bool isCastling() const { return data & 0x800000; }
        
        // Convert to algebraic notation for compatibility with existing Board class
        std::string toAlgebraic() const;
    };
    
    // Pre-allocated move list to avoid heap allocation
    struct MoveList {
        Move moves[256];
        int count = 0;
        
        void add(const Move& move) {
            if (count < 256) moves[count++] = move;
        }
        
        void clear() { count = 0; }
    };
    
    FastMoveGenerator();
    
    // Generate all pseudo-legal moves
    void generateMoves(const Board& board, bool isWhite, MoveList& moveList) const;
    
    // Generate only legal moves (filters out moves that leave king in check)
    void generateLegalMoves(const Board& board, bool isWhite, MoveList& moveList) const;
    
    // Piece-specific move generation methods (public for MoveGenerator interface)
    void generatePawnMoves(const Board& board, bool isWhite, MoveList& moveList) const;
    void generateKnightMoves(const Board& board, bool isWhite, MoveList& moveList) const;
    void generateBishopMoves(const Board& board, bool isWhite, MoveList& moveList) const;
    void generateRookMoves(const Board& board, bool isWhite, MoveList& moveList) const;
    void generateQueenMoves(const Board& board, bool isWhite, MoveList& moveList) const;
    void generateKingMoves(const Board& board, bool isWhite, MoveList& moveList) const;
    
    // Check if a square is attacked by the given side
    bool isSquareAttacked(const Board& board, int square, bool byWhite) const;
    
    // Check if king is in check
    bool isKingInCheck(const Board& board, bool whiteKing) const;
    
    // Check if a specific move is legal (doesn't leave king in check)
    bool isMoveLegal(const Board& board, const Move& move, bool isWhite) const;
    
private:
    // Pre-computed attack tables (like BBC)
    uint64_t knightAttacks[64];
    uint64_t kingAttacks[64];
    uint64_t pawnAttacks[2][64]; // [color][square]
    
    void initAttackTables();
    
    // Inline helper functions for performance
    inline int lsbIndex(uint64_t bb) const {
#ifdef _MSC_VER
        unsigned long idx;
        _BitScanForward64(&idx, bb);
        return idx;
#else
        return __builtin_ctzll(bb);
#endif
    }
    
    inline uint64_t popLSB(uint64_t& bb) const {
        uint64_t lsb = bb & -bb;
        bb &= bb - 1;
        return lsb;
    }
    
    // Internal castling move generation
    void generateCastlingMoves(const Board& board, bool isWhite, MoveList& moveList) const;
};
