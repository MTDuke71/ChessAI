#include "../src/MoveGenerator.h"
#include "../src/Board.h"
#include "../src/Perft.h"
#include <iostream>
#include <chrono>

int main() {
    std::cout << "FastMoveGenerator Perft Validation Test\n";
    std::cout << "======================================\n\n";
    
    try {
        Board board;
        MoveGenerator moveGen;
        
        std::cout << "Testing FastMoveGenerator with known perft values...\n\n";
        
        // Test perft depth 1
        std::cout << "Depth 1: ";
        std::cout.flush();
        auto start = std::chrono::high_resolution_clock::now();
        uint64_t nodes1 = perft(board, moveGen, 1);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << nodes1 << " nodes (" << duration1.count() << " μs)\n";
        std::cout << "Expected: 20 nodes - " << (nodes1 == 20 ? "✓ PASS" : "✗ FAIL") << "\n\n";
        
        // Test perft depth 2
        std::cout << "Depth 2: ";
        std::cout.flush();
        start = std::chrono::high_resolution_clock::now();
        uint64_t nodes2 = perft(board, moveGen, 2);
        end = std::chrono::high_resolution_clock::now();
        auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << nodes2 << " nodes (" << duration2.count() << " μs)\n";
        std::cout << "Expected: 400 nodes - " << (nodes2 == 400 ? "✓ PASS" : "✗ FAIL") << "\n\n";
        
        // Test perft depth 3
        std::cout << "Depth 3: ";
        std::cout.flush();
        start = std::chrono::high_resolution_clock::now();
        uint64_t nodes3 = perft(board, moveGen, 3);
        end = std::chrono::high_resolution_clock::now();
        auto duration3 = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << nodes3 << " nodes (" << duration3.count() << " μs)\n";
        std::cout << "Expected: 8,902 nodes - " << (nodes3 == 8902 ? "✓ PASS" : "✗ FAIL") << "\n\n";
        
        // Test perft depth 4
        std::cout << "Depth 4: ";
        std::cout.flush();
        start = std::chrono::high_resolution_clock::now();
        uint64_t nodes4 = perft(board, moveGen, 4);
        end = std::chrono::high_resolution_clock::now();
        auto duration4 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << nodes4 << " nodes (" << duration4.count() << " ms)\n";
        std::cout << "Expected: 197,281 nodes - " << (nodes4 == 197281 ? "✓ PASS" : "✗ FAIL") << "\n\n";
        
        // Performance summary
        std::cout << "Performance Summary:\n";
        std::cout << "- Depth 1: " << (duration1.count() / 1000.0) << " ms\n";
        std::cout << "- Depth 2: " << (duration2.count() / 1000.0) << " ms\n";
        std::cout << "- Depth 3: " << (duration3.count() / 1000.0) << " ms\n";
        std::cout << "- Depth 4: " << duration4.count() << " ms\n\n";
        
        if (nodes1 == 20 && nodes2 == 400 && nodes3 == 8902 && nodes4 == 197281) {
            std::cout << "🎉 All perft tests PASSED! FastMoveGenerator integration successful!\n";
            
            // Calculate approximate NPS for depth 4
            double nps = (double)nodes4 / (duration4.count() / 1000.0);
            std::cout << "Approximate NPS: " << (int)(nps / 1000) << "K nodes/second\n";
        } else {
            std::cout << "❌ Some perft tests FAILED! There may be bugs in FastMoveGenerator.\n";
        }
        
    } catch (const std::exception& e) {
        std::cout << "\n✗ ERROR: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cout << "\n✗ Unknown error occurred!\n";
        return 1;
    }
    
    return 0;
}
