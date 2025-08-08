#include "../src/Board.h"
#include "../src/MoveGenerator.h" 
#include "../src/MoveEncoding.h"
#include <iostream>

int main() {
    std::cout << "=== TESTING ACTUAL MAKEMOVE ===\n";
    
    Board board;
    board.loadFEN("4k3/8/8/8/8/8/8/4K2R w K - 0 1");
    
    std::cout << "Original position:\n";
    board.printBoard();
    
    uint16_t castlingMove = encodeMove("e1-g1");
    
    std::cout << "\nTesting isMoveLegal directly:\n";
    bool legal = board.isMoveLegal(castlingMove);
    std::cout << "isMoveLegal result: " << (legal ? "LEGAL" : "ILLEGAL") << "\n";
    
    std::cout << "\nNow attempting makeMove:\n";
    board.makeMove(castlingMove);
    
    std::cout << "\nPosition after makeMove attempt:\n";
    board.printBoard();
    
    std::cout << "\nCastling rights after makeMove:\n";
    std::cout << "White KS: " << (board.canCastleWK() ? "YES" : "NO") << "\n";
    
    return 0;
}
