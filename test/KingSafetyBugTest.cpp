#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

int main() {
    std::cout << "=== KING SAFETY BUG REPRODUCTION ===\n";
    
    // Load the problematic position
    Board board;
    board.loadFEN("rnbqkbnr/ppppp1pp/8/8/4Pp2/8/PPPPKPPP/RNBQ1BNR w kq - 0 3");
    
    std::cout << "Position: rnbqkbnr/ppppp1pp/8/8/4Pp2/8/PPPPKPPP/RNBQ1BNR w kq - 0 3\n";
    std::cout << "White to move\n\n";
    
    // Generate moves with FastMoveGenerator
    MoveGenerator gen;
    auto moves = gen.generateAllMoves(board, true);
    
    std::cout << "Generated moves:\n";
    for (auto move : moves) {
        std::string moveStr = decodeMove(move);
        bool legal = board.isMoveLegal(moveStr);
        std::cout << moveStr << " - " << (legal ? "LEGAL" : "ILLEGAL") << "\n";
    }
    
    // Specifically test e2e3
    std::cout << "\n=== TESTING e2e3 SPECIFICALLY ===\n";
    bool e2e3Legal = board.isMoveLegal("e2-e3");
    std::cout << "e2-e3 legality check: " << (e2e3Legal ? "LEGAL" : "ILLEGAL") << "\n";
    
    // Try to make the move and see what happens
    try {
        Board testBoard = board;
        Board::MoveState state;
        testBoard.makeMove("e2-e3", state);
        std::cout << "Move e2-e3 was accepted by makeMove\n";
        
        MoveGenerator checkGen;
        bool inCheck = checkGen.isKingInCheck(testBoard, true);
        std::cout << "After e2-e3, white king in check: " << (inCheck ? "YES" : "NO") << "\n";
        
    } catch (const std::exception& e) {
        std::cout << "Move e2-e3 was rejected by makeMove: " << e.what() << "\n";
    }
    
    return 0;
}
