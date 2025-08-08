#include "../src/Board.h"
#include "../src/Perft.h"
#include "../src/MoveGenerator.h"
#include <iostream>
#include <iomanip>

int main() {
    std::cout << "=== SIMPLE DEPTH 3 TEST ===\n";
    
    Board board;
    board.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    MoveGenerator generator;
    
    std::cout << "FEN: " << board.getFEN() << "\n";
    
    auto result1 = perft(board, generator, 1);
    auto result2 = perft(board, generator, 2);  
    auto result3 = perft(board, generator, 3);
    
    std::cout << "Depth 1: " << result1 << " (expected: 48)\n";
    std::cout << "Depth 2: " << result2 << " (expected: 2039)\n";
    std::cout << "Depth 3: " << result3 << " (expected: 97862)\n";
    
    std::cout << "Accuracy D1: " << std::fixed << std::setprecision(2) 
              << (100.0 * result1 / 48) << "%\n";
    std::cout << "Accuracy D2: " << std::fixed << std::setprecision(2) 
              << (100.0 * result2 / 2039) << "%\n";
    std::cout << "Accuracy D3: " << std::fixed << std::setprecision(2) 
              << (100.0 * result3 / 97862) << "%\n";
    
    int deficit3 = 97862 - result3;
    std::cout << "Depth 3 deficit: " << deficit3 << " nodes\n";
    
    return 0;
}
