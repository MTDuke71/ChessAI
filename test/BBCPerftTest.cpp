#include <iostream>
#include <chrono>

// BBC includes
extern "C" {
#include "../BBC/src/BitBoard.h"
#include "../BBC/src/TimeControl.h"
#include "../BBC/src/Random.h"
#include "../BBC/src/Hash.h"
#include "../BBC/src/Board.h"
#include "../BBC/src/Attacks.h"
#include "../BBC/src/MagicBitboards.h"
#include "../BBC/src/MoveGeneration.h"
#include "../BBC/src/Perft.h"
#include "../BBC/src/Evaluation.h"
#include "../BBC/src/Search.h"
}

int main() {
    std::cout << "BBC Engine Perft Performance Test\n";
    std::cout << "==================================\n\n";
    
    // Initialize BBC engine
    init_all();
    
    // Set up starting position
    parse_fen(start_position);
    
    std::cout << "Testing Perft on starting position:\n";
    print_board();
    
    // Test depths 1-7 to get meaningful performance data
    for (int depth = 1; depth <= 7; depth++) {
        std::cout << "\nTesting depth " << depth << ":\n";
        
        // Reset node counter
        extern U64 nodes;
        nodes = 0;
        
        // Time the perft test
        auto start = std::chrono::high_resolution_clock::now();
        perft_driver(depth);
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        double seconds = duration.count() / 1000000.0;
        
        std::cout << "  Nodes: " << nodes << std::endl;
        std::cout << "  Time: " << seconds << " seconds" << std::endl;
        
        if (seconds > 0) {
            std::cout << "  NPS: " << (long)(nodes / seconds) << " nodes/second" << std::endl;
        }
    }
    
    std::cout << "\nNote: Both BBC.exe and BBC_modular.exe use the same algorithms\n";
    std::cout << "and should have identical performance characteristics.\n";
    std::cout << "The modular version provides better code organization\n";
    std::cout << "without sacrificing speed when compiled with optimization.\n";
    
    return 0;
}
