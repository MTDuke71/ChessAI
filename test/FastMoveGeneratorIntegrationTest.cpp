#include "../src/MoveGenerator.h"
#include "../src/Board.h"
#include <iostream>
#include <chrono>

int main() {
    std::cout << "FastMoveGenerator Integration Test\n";
    std::cout << "==================================\n\n";
    
    try {
        // Create a board and move generator
        Board board;
        MoveGenerator moveGen;
        
        std::cout << "1. Testing initial position move generation...\n";
        auto moves = moveGen.generateLegalMoves(board, true);
        std::cout << "   Generated " << moves.size() << " legal moves for white in starting position\n";
        std::cout << "   Expected: 20 moves\n";
        
        if (moves.size() == 20) {
            std::cout << "   ✓ PASS - Correct number of moves\n";
        } else {
            std::cout << "   ✗ FAIL - Wrong number of moves\n";
        }
        
        std::cout << "\n2. Testing some specific moves:\n";
        for (size_t i = 0; i < std::min(moves.size(), (size_t)5); i++) {
            std::cout << "   Move " << (i+1) << ": " << std::hex << moves[i] << std::dec << "\n";
        }
        
        std::cout << "\n3. Testing performance (1000 move generations)...\n";
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 1000; i++) {
            auto testMoves = moveGen.generateLegalMoves(board, true);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "   Time for 1000 generations: " << duration.count() << " microseconds\n";
        std::cout << "   Average per generation: " << (duration.count() / 1000.0) << " microseconds\n";
        
        std::cout << "\n4. Testing different piece types...\n";
        auto pawnMoves = moveGen.generatePawnMoves(board, true);
        auto knightMoves = moveGen.generateKnightMoves(board, true);
        auto rookMoves = moveGen.generateRookMoves(board, true);
        auto bishopMoves = moveGen.generateBishopMoves(board, true);
        auto queenMoves = moveGen.generateQueenMoves(board, true);
        auto kingMoves = moveGen.generateKingMoves(board, true);
        
        std::cout << "   Pawn moves: " << pawnMoves.size() << "\n";
        std::cout << "   Knight moves: " << knightMoves.size() << "\n";
        std::cout << "   Rook moves: " << rookMoves.size() << "\n";
        std::cout << "   Bishop moves: " << bishopMoves.size() << "\n";
        std::cout << "   Queen moves: " << queenMoves.size() << "\n";
        std::cout << "   King moves: " << kingMoves.size() << "\n";
        
        std::cout << "\n✓ Integration test completed successfully!\n";
        
    } catch (const std::exception& e) {
        std::cout << "\n✗ ERROR: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cout << "\n✗ Unknown error occurred!\n";
        return 1;
    }
    
    return 0;
}
