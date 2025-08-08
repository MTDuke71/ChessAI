#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::cout << "=== PERFT DEPTH 1 TEST ===\n";
    
    Board board;
    board.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1");
    
    std::cout << "FEN: " << board.getFEN() << "\n";
    std::cout << "This is the position after a2-a4 from the test position\n";
    std::cout << "Expected: 44 legal moves for black\n";
    std::cout << "Engine produces: 45 moves\n\n";
    
    MoveGenerator gen;
    
    // Test with built-in perft
    uint64_t perftResult = perft(board, gen, 1);
    std::cout << "Built-in perft(1): " << perftResult << "\n\n";
    
    // Generate all moves and check legality
    auto moves = gen.generateAllMoves(board, false); // Black to move
    
    std::cout << "Generated pseudo-legal moves: " << moves.size() << "\n";
    
    std::vector<std::string> legalMoves;
    std::vector<std::string> illegalMoves;
    
    for (auto move : moves) {
        std::string moveStr = decodeMove(move);
        if (board.isMoveLegal(moveStr)) {
            legalMoves.push_back(moveStr);
        } else {
            illegalMoves.push_back(moveStr);
        }
    }
    
    std::cout << "Legal moves: " << legalMoves.size() << "\n";
    std::cout << "Illegal moves filtered: " << illegalMoves.size() << "\n\n";
    
    // Show all legal moves
    std::cout << "All legal moves:\n";
    std::sort(legalMoves.begin(), legalMoves.end());
    for (size_t i = 0; i < legalMoves.size(); ++i) {
        std::cout << legalMoves[i];
        if ((i + 1) % 8 == 0) std::cout << "\n";
        else std::cout << " ";
    }
    std::cout << "\n\n";
    
    // Show illegal moves (the extra one)
    if (!illegalMoves.empty()) {
        std::cout << "Illegal moves that should be filtered:\n";
        for (const std::string& move : illegalMoves) {
            std::cout << move << " ";
        }
        std::cout << "\n";
    }
    
    return 0;
}
