#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/FastMoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>
#include <vector>
#include <map>

// Perft function for a single move using original system
uint64_t perftOriginal(Board& board, int depth, uint16_t move) {
    if (depth == 0) return 1;
    
    Board::MoveState state;
    board.makeMove(move, state);
    
    MoveGenerator moveGen;
    std::vector<uint16_t> moves = moveGen.generateLegalMoves(board, board.isWhiteToMove());
    
    uint64_t nodes = 0;
    for (uint16_t nextMove : moves) {
        nodes += perftOriginal(board, depth - 1, nextMove);
    }
    
    board.unmakeMove(state);
    return nodes;
}

// Perft function for a single move using FastMoveGenerator
uint64_t perftFast(Board& board, int depth, const FastMoveGenerator::Move& move) {
    if (depth == 0) return 1;
    
    Board::MoveState state;
    std::string moveStr = move.toAlgebraic();
    board.makeMove(moveStr, state);
    
    FastMoveGenerator fastGen;
    FastMoveGenerator::MoveList moves;
    fastGen.generateLegalMoves(board, board.isWhiteToMove(), moves);
    
    uint64_t nodes = 0;
    for (int i = 0; i < moves.count; i++) {
        nodes += perftFast(board, depth - 1, moves.moves[i]);
    }
    
    board.unmakeMove(state);
    return nodes;
}

int main() {
    Board board;
    MoveGenerator originalGen;
    FastMoveGenerator fastGen;
    
    // Load the problematic position
    board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    std::cout << "PERFT DIVIDE ANALYSIS - DEPTH 5\n";
    std::cout << "===============================\n\n";
    std::cout << "Position: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\n\n";
    
    // Get moves from both systems
    std::vector<uint16_t> originalMoves = originalGen.generateLegalMoves(board, true);
    FastMoveGenerator::MoveList fastMoves;
    fastGen.generateLegalMoves(board, true, fastMoves);
    
    std::cout << "Original system moves: " << originalMoves.size() << "\n";
    std::cout << "Fast system moves: " << fastMoves.count << "\n\n";
    
    // Calculate perft for each original move
    std::cout << "ORIGINAL SYSTEM PERFT DIVIDE:\n";
    uint64_t totalOriginal = 0;
    std::map<std::string, uint64_t> originalResults;
    
    for (size_t i = 0; i < originalMoves.size(); i++) {
        Board tempBoard = board;
        std::string moveStr = decodeMove(originalMoves[i]);
        uint64_t nodes = perftOriginal(tempBoard, 4, originalMoves[i]); // depth 4 since we already made 1 move
        originalResults[moveStr] = nodes;
        totalOriginal += nodes;
        std::cout << "  " << moveStr << ": " << nodes << "\n";
    }
    std::cout << "Total: " << totalOriginal << "\n\n";
    
    // Calculate perft for each fast move
    std::cout << "FAST SYSTEM PERFT DIVIDE:\n";
    uint64_t totalFast = 0;
    std::map<std::string, uint64_t> fastResults;
    
    for (int i = 0; i < fastMoves.count; i++) {
        Board tempBoard = board;
        std::string moveStr = fastMoves.moves[i].toAlgebraic();
        uint64_t nodes = perftFast(tempBoard, 4, fastMoves.moves[i]); // depth 4 since we already made 1 move
        fastResults[moveStr] = nodes;
        totalFast += nodes;
        std::cout << "  " << moveStr << ": " << nodes << "\n";
    }
    std::cout << "Total: " << totalFast << "\n\n";
    
    // Find differences
    std::cout << "DIFFERENCES:\n";
    std::cout << "============\n";
    
    bool foundDifferences = false;
    
    // Check for moves only in fast system
    for (const auto& pair : fastResults) {
        if (originalResults.find(pair.first) == originalResults.end()) {
            std::cout << "EXTRA MOVE in Fast system: " << pair.first << " (" << pair.second << " nodes)\n";
            foundDifferences = true;
        }
    }
    
    // Check for moves only in original system  
    for (const auto& pair : originalResults) {
        if (fastResults.find(pair.first) == fastResults.end()) {
            std::cout << "MISSING MOVE in Fast system: " << pair.first << " (" << pair.second << " nodes)\n";
            foundDifferences = true;
        }
    }
    
    // Check for moves with different node counts
    for (const auto& pair : originalResults) {
        auto it = fastResults.find(pair.first);
        if (it != fastResults.end() && it->second != pair.second) {
            std::cout << "DIFFERENT NODE COUNT for " << pair.first << ": Original=" << pair.second << ", Fast=" << it->second << "\n";
            foundDifferences = true;
        }
    }
    
    if (!foundDifferences) {
        std::cout << "No differences found in individual moves.\n";
    }
    
    std::cout << "\nTotal difference: " << (int64_t)totalFast - (int64_t)totalOriginal << " nodes\n";
    
    return 0;
}
