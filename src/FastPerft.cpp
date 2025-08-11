#include "FastPerft.h"
#include "IncrementalBoard.h"
#include <chrono>
#include <iostream>

// BBC-style ultra-fast perft implementation
uint64_t FastPerft::perft(const Board& board, int depth) {
    FastMoveGenerator gen;
    IncrementalBoard incBoard(board);
    return perftRecursive(gen, incBoard, depth);
}

uint64_t FastPerft::perftDivide(const Board& board, int depth) {
    FastMoveGenerator gen;
    FastMoveGenerator::MoveList moves;
    
    // Generate legal moves directly
    gen.generateLegalMoves(board, board.isWhiteToMove(), moves);
    
    uint64_t total = 0;
    IncrementalBoard incBoard(board);
    
    for (int i = 0; i < moves.count; i++) {
        const auto& move = moves.moves[i];
        
        // Apply move using incremental board (BBC-style)
        IncrementalBoard::MoveState state;
        // For divide, we need to temporarily modify our incremental board approach
        // For now, fall back to regular perft for divide functionality
        Board testBoard = board;
        Board::MoveState boardState;
        testBoard.makeMove(move.toAlgebraic(), boardState);
        
        uint64_t nodes = 0;
        if (!gen.isKingInCheck(testBoard, testBoard.isWhiteToMove())) {
            nodes = perft(testBoard, depth - 1);
            std::cout << move.toAlgebraic() << ": " << nodes << "\n";
        }
        
        total += nodes;
        testBoard.unmakeMove(boardState);
    }
    
    return total;
}

uint64_t FastPerft::perftTimed(const Board& board, int depth, double& ms) {
    auto start = std::chrono::steady_clock::now();
    uint64_t nodes = perft(board, depth);
    auto end = std::chrono::steady_clock::now();
    ms = std::chrono::duration<double, std::milli>(end - start).count();
    return nodes;
}

// Core BBC-style recursive perft - ultra-fast, no string conversions
uint64_t FastPerft::perftRecursive(FastMoveGenerator& gen, IncrementalBoard& board, int depth) {
    if (depth == 0) return 1;
    
    FastMoveGenerator::MoveList moves;
    // Generate legal moves directly using our BBC-style approach
    gen.generateLegalMoves(board, board.isWhiteToMove(), moves);
    
    if (depth == 1) {
        return moves.count; // All moves are already legal
    }
    
    uint64_t nodes = 0;
    
    for (int i = 0; i < moves.count; i++) {
        const auto& move = moves.moves[i];
        
        // This is where we need to enhance IncrementalBoard to support
        // FastMoveGenerator::Move directly for maximum BBC-style speed
        // For now, convert to basic parameters
        int special = move.isCastling() ? 3 : (move.promotion() ? 1 : 0);
        
        // BBC-style make/unmake using IncrementalBoard
        IncrementalBoard::MoveState state;
        board.applyMoveIncremental(move.from(), move.to(), special, move.promotion(), state);
        
        // Recurse
        nodes += perftRecursive(gen, board, depth - 1);
        
        // BBC-style rollback
        board.rollbackMove(state, move.from(), move.to(), special, move.promotion());
    }
    
    return nodes;
}
