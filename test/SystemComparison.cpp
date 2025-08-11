#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/FastMoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>
#include <vector>

int main() {
    Board board;
    MoveGenerator originalGen;
    FastMoveGenerator fastGen;
    
    // Test standard starting position
    board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    std::cout << "STANDARD STARTING POSITION TEST\n";
    std::cout << "===============================\n\n";
    
    // Generate moves with both systems
    std::vector<uint16_t> originalMoves = originalGen.generateLegalMoves(board, true);
    FastMoveGenerator::MoveList fastMoves;
    fastGen.generateLegalMoves(board, true, fastMoves);
    
    std::cout << "Standard position: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\n";
    std::cout << "Original MoveGenerator: " << originalMoves.size() << " moves\n";
    std::cout << "FastMoveGenerator: " << fastMoves.count << " moves\n\n";
    
    if (originalMoves.size() != fastMoves.count) {
        std::cout << "DISCREPANCY FOUND in starting position!\n";
        std::cout << "Original moves:\n";
        for (size_t i = 0; i < originalMoves.size(); i++) {
            std::cout << "  " << (i+1) << ". " << decodeMove(originalMoves[i]) << "\n";
        }
        std::cout << "\nFast moves:\n";
        for (int i = 0; i < fastMoves.count; i++) {
            std::cout << "  " << (i+1) << ". " << fastMoves.moves[i].toAlgebraic() << "\n";
        }
    } else {
        std::cout << "Both systems agree on starting position: " << originalMoves.size() << " moves\n";
    }
    
    // Now test our problem position
    std::cout << "\n\nPROBLEM POSITION TEST\n";
    std::cout << "====================\n\n";
    
    board.loadFEN("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    
    originalMoves = originalGen.generateLegalMoves(board, true);
    fastMoves.clear(); // Reset
    fastGen.generateLegalMoves(board, true, fastMoves);
    
    std::cout << "Problem position: r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1\n";
    std::cout << "Original: " << originalMoves.size() << " moves\n";
    std::cout << "Fast: " << fastMoves.count << " moves\n";
    std::cout << "Difference: " << (int)fastMoves.count - (int)originalMoves.size() << " moves\n\n";
    
    return 0;
}
