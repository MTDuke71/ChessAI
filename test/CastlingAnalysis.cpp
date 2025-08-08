#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::cout << "=== KINGSIDE CASTLING ANALYSIS ===\n";
    
    Board board;
    board.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    std::cout << "Original FEN: " << board.getFEN() << "\n";
    
    // Make the kingside castling move
    Board::MoveState state;
    board.makeMove("O-O", state);
    
    std::cout << "After O-O FEN: " << board.getFEN() << "\n";
    
    MoveGenerator gen;
    
    // Count using built-in perft
    uint64_t perftResult = perft(board, gen, 1);
    std::cout << "Built-in perft(1) after O-O: " << perftResult << "\n";
    
    // Count manually
    auto moves = gen.generateAllMoves(board, false); // Black to move
    uint64_t manualCount = 0;
    
    std::cout << "\nBlack moves after O-O:\n";
    std::vector<std::pair<std::string, int>> moveList;
    
    for (auto move : moves) {
        std::string moveStr = decodeMove(move);
        if (board.isMoveLegal(moveStr)) {
            manualCount++;
            moveList.push_back({moveStr, 1});
        }
    }
    
    std::cout << "Manual count: " << manualCount << "\n";
    std::cout << "Expected: 43 (from depth 2 analysis)\n";
    
    // Sort and display moves
    std::sort(moveList.begin(), moveList.end());
    for (size_t i = 0; i < moveList.size(); ++i) {
        std::cout << moveList[i].first;
        if ((i + 1) % 8 == 0) std::cout << "\n";
        else std::cout << " ";
    }
    std::cout << "\n";
    
    // Now test depth 2 from this position
    std::cout << "\n=== DEPTH 2 FROM CASTLED POSITION ===\n";
    uint64_t depth2Result = perft(board, gen, 2);
    std::cout << "Built-in perft(2) after O-O: " << depth2Result << "\n";
    std::cout << "Expected: 2059\n";
    std::cout << "Difference: " << (int64_t)depth2Result - 2059 << "\n";
    
    return 0;
}
