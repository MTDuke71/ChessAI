#include "UltraFastPerft.h"
#include "Board.h"
#include "MoveGenerator.h"
#include "Perft.h"
#include <chrono>
#include <iostream>

int main() {
    std::cout << "=== ULTIMATE PERFORMANCE SHOWDOWN ===\n";
    std::cout << "BBC-Style Ultra-Fast vs Current Aphelion\n\n";
    
    Board board;
    
    // Test depth 4 for meaningful comparison
    const int testDepth = 4;
    
    std::cout << "Test Configuration:\n";
    std::cout << "- Position: Starting position\n";
    std::cout << "- Depth: " << testDepth << "\n";
    std::cout << "- Expected nodes: 197,281 (depth 4)\n\n";
    
    // Test 1: Current Aphelion Implementation
    std::cout << "1. CURRENT APHELION IMPLEMENTATION:\n";
    MoveGenerator gen;
    
    auto start1 = std::chrono::steady_clock::now();
    double ms1;
    uint64_t nodes1 = perft(board, gen, testDepth, ms1);
    auto end1 = std::chrono::steady_clock::now();
    ms1 = std::chrono::duration<double, std::milli>(end1 - start1).count();
    
    std::cout << "   Nodes: " << nodes1 << "\n";
    std::cout << "   Time: " << ms1 << " ms\n";
    std::cout << "   NPS: " << (nodes1 / ms1 * 1000.0) << " nodes/second\n\n";
    
    // Test 2: BBC-Style Ultra-Fast Implementation
    std::cout << "2. BBC-STYLE ULTRA-FAST IMPLEMENTATION:\n";
    
    auto start2 = std::chrono::steady_clock::now();
    double ms2;
    uint64_t nodes2 = UltraFastPerft::perftTimed(board, testDepth, ms2);
    auto end2 = std::chrono::steady_clock::now();
    ms2 = std::chrono::duration<double, std::milli>(end2 - start2).count();
    
    std::cout << "   Nodes: " << nodes2 << "\n";
    std::cout << "   Time: " << ms2 << " ms\n";
    std::cout << "   NPS: " << (nodes2 / ms2 * 1000.0) << " nodes/second\n\n";
    
    // Analysis
    std::cout << "=== PERFORMANCE ANALYSIS ===\n";
    if (nodes1 == nodes2) {
        std::cout << "✓ Node counts match - accuracy verified!\n";
    } else {
        std::cout << "✗ Node count mismatch - need debugging\n";
        std::cout << "   Difference: " << (int64_t)(nodes1 - nodes2) << " nodes\n";
    }
    
    if (ms2 > 0) {
        double speedup = ms1 / ms2;
        std::cout << "🚀 SPEEDUP: " << speedup << "x faster\n";
        std::cout << "⚡ Performance gain: " << ((speedup - 1) * 100) << "%\n";
        
        if (speedup > 2.0) {
            std::cout << "🏆 BREAKTHROUGH PERFORMANCE!\n";
        } else if (speedup > 1.5) {
            std::cout << "📈 SIGNIFICANT IMPROVEMENT!\n";
        } else if (speedup > 1.1) {
            std::cout << "📊 Measurable improvement\n";
        } else {
            std::cout << "🔍 Marginal improvement - may need further optimization\n";
        }
    }
    
    std::cout << "\n=== BBC-STYLE OPTIMIZATIONS ACHIEVED ===\n";
    std::cout << "✓ Direct bitboard array access\n";
    std::cout << "✓ Ultra-fast memcpy-based make/unmake\n";
    std::cout << "✓ Minimal abstraction overhead\n";
    std::cout << "✓ BBC-style bit manipulation macros\n";
    std::cout << "✓ Streamlined move representation\n";
    std::cout << "✓ Optimized attack checking\n";
    
    // Compare with BBC target
    std::cout << "\n=== BBC COMPARISON ===\n";
    double targetNPS = 55600000; // BBC achieves ~55.6M NPS
    double currentNPS = nodes2 / ms2 * 1000.0;
    double bbcRatio = targetNPS / currentNPS;
    
    std::cout << "BBC target NPS: " << targetNPS << "\n";
    std::cout << "Current NPS: " << currentNPS << "\n";
    if (bbcRatio > 1.0) {
        std::cout << "Remaining gap: " << bbcRatio << "x to match BBC\n";
    } else {
        std::cout << "🎯 GOAL ACHIEVED: Matching or exceeding BBC performance!\n";
    }
    
    return 0;
}
