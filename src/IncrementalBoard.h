#pragma once
#include "Board.h"
#include <cstdint>

// BBC-style incremental board state for ultra-fast legal move checking
// This avoids the expensive Board copy + applyMove + check approach
class IncrementalBoard {
public:
    IncrementalBoard(const Board& board);
    
    // BBC-style approach: Apply move incrementally, check legality, rollback if needed
    bool isMoveLegal(int from, int to, int special = 0, int promotion = 0);
    
private:
    // Lightweight state for fast rollback
    struct MoveState {
        uint64_t capturedPiece;
        int capturedSquare;
        uint64_t originalCastleRights;
        int originalEnPassant;
        bool originalWhiteToMove;
    };
    
    // Core bitboards (copied from Board for speed)
    uint64_t whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing;
    uint64_t blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing;
    
    // State
    bool whiteToMove;
    int enPassantSquare;
    bool castleWK, castleWQ, castleBK, castleBQ;
    
    // Fast occupancy computation
    uint64_t getWhitePieces() const;
    uint64_t getBlackPieces() const;
    uint64_t getAllPieces() const;
    
    // BBC-style incremental move application
    void applyMoveIncremental(int from, int to, int special, int promotion, MoveState& state);
    void rollbackMove(const MoveState& state, int from, int to, int special, int promotion);
    
    // Ultra-fast attack checking (BBC-style)
    bool isSquareAttackedFast(int square, bool byWhite) const;
    bool isKingInCheckFast(bool whiteKing) const;
    
    // Helper to get piece type at square
    int getPieceAt(int square) const;
};
