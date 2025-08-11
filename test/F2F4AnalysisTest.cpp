#include <iostream>
#include "Board.h"
#include "FastMoveGenerator.h"
#include "MoveGenerator.h"
#include "MoveEncoding.h"

int main() {
    std::cout << "Analyzing f2-f4 Move Legality\n";
    std::cout << "=============================\n\n";
    
    Board board;
    
    // Recreate the position: e2-e4 e7-e5 g1-f3 b8-c6
    std::vector<std::string> moves = {"e2-e4", "e7-e5", "g1-f3", "b8-c6"};
    std::vector<Board::MoveState> states;
    
    for (const auto& moveStr : moves) {
        Board::MoveState state;
        board.makeMove(moveStr, state);
        states.push_back(state);
        std::cout << "Made move: " << moveStr << "\n";
    }
    
    std::cout << "\nAnalyzing f2-f4 move:\n";
    
    // Test if f2-f4 is legal by trying to make it
    try {
        Board::MoveState testState;
        std::cout << "Attempting to make f2-f4...\n";
        board.makeMove("f2-f4", testState);
        std::cout << "✅ f2-f4 was successfully made\n";
        
        // Check if white king is in check after f2-f4
        FastMoveGenerator fastGen;
        bool kingInCheck = fastGen.isKingInCheck(board, true);
        std::cout << "White king in check after f2-f4: " << (kingInCheck ? "YES" : "NO") << "\n";
        
        board.unmakeMove(testState);
        
    } catch (const std::exception& e) {
        std::cout << "❌ f2-f4 failed to make: " << e.what() << "\n";
    } catch (...) {
        std::cout << "❌ f2-f4 failed to make (unknown error)\n";
    }
    
    // Let's also check if the white king is already in check before f2-f4
    FastMoveGenerator fastGen;
    bool kingInCheckBefore = fastGen.isKingInCheck(board, true);
    std::cout << "White king in check before f2-f4: " << (kingInCheckBefore ? "YES" : "NO") << "\n";
    
    // Show board position for analysis
    std::cout << "\nBoard position analysis:\n";
    std::cout << "White king position: " << std::hex << board.getWhiteKing() << std::dec << "\n";
    std::cout << "Black pieces threatening: \n";
    std::cout << "  Black bishops: " << std::hex << board.getBlackBishops() << std::dec << "\n";
    std::cout << "  Black queens: " << std::hex << board.getBlackQueens() << std::dec << "\n";
    std::cout << "  Black knights: " << std::hex << board.getBlackKnights() << std::dec << "\n";
    
    return 0;
}
