#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

void printMove(uint16_t move) {
    // Extract move information manually since we can't use decodeMove
    int from = (move >> 6) & 0x3F;
    int to = move & 0x3F;
    
    char fromFile = 'a' + (from % 8);
    char fromRank = '1' + (from / 8);
    char toFile = 'a' + (to % 8);
    char toRank = '1' + (to / 8);
    
    std::cout << fromFile << fromRank << "-" << toFile << toRank;
}

int main() {
    std::cout << "=== MANUAL MOVE ANALYSIS FOR POSITION 3 ===\n";
    
    Board board;
    MoveGenerator generator;
    
    std::string fen = "4k3/8/8/8/8/8/8/4K2R w K - 0 1";
    
    if (!board.loadFEN(fen)) {
        std::cerr << "Failed to load FEN\n";
        return 1;
    }
    
    std::cout << "Position: " << fen << "\n";
    board.printBoard();
    
    std::cout << "\nCastling rights:\n";
    std::cout << "White KS: " << (board.canCastleWK() ? "YES" : "NO") << "\n";
    std::cout << "White QS: " << (board.canCastleWQ() ? "YES" : "NO") << "\n";
    std::cout << "Black KS: " << (board.canCastleBK() ? "YES" : "NO") << "\n";
    std::cout << "Black QS: " << (board.canCastleBQ() ? "YES" : "NO") << "\n";
    
    auto moves = generator.generateAllMoves(board, true);
    
    std::cout << "\nGenerated moves (" << moves.size() << " total):\n";
    
    for (size_t i = 0; i < moves.size(); ++i) {
        std::cout << i+1 << ". ";
        printMove(moves[i]);
        
        // Check if this is castling
        if (moves[i] == encodeMove("e1-g1")) {
            std::cout << " [CASTLING]";
        }
        
        std::cout << "\n";
    }
    
    return 0;
}
