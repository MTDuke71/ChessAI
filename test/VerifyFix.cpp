#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/Perft.h"
#include <iostream>

int main() {
    Board board;
    MoveGenerator gen;
    board.loadFEN("4k3/8/8/8/8/8/8/4K2R w K - 0 1");
    
    std::cout << "=== POSITION 3 PERFT VERIFICATION ===" << std::endl;
    std::cout << "Position: 4k3/8/8/8/8/8/8/4K2R w K - 0 1" << std::endl;
    std::cout << std::endl;
    
    for (int depth = 1; depth <= 6; depth++) {
        uint64_t nodes = perft(board, gen, depth);
        std::cout << "Perft(" << depth << ") = " << nodes << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Expected Perft(6) = 764643" << std::endl;
    std::cout << "Previous result = 764613 (30 nodes missing)" << std::endl;
    uint64_t actual = perft(board, gen, 6);
    std::cout << "Current result = " << actual << std::endl;
    
    if (actual == 764643) {
        std::cout << "✅ BUG FIXED! Perfect match!" << std::endl;
    } else {
        std::cout << "❌ Still " << (764643 > actual ? 764643 - actual : actual - 764643) << " nodes off" << std::endl;
    }
    
    return 0;
}
