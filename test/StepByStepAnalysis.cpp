#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include "../src/Perft.h"
#include <iostream>

int main() {
    std::cout << "=== STEP-BY-STEP CASTLING PATH ANALYSIS ===\n";
    
    // Start with original position
    Board board;
    board.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    std::cout << "1. Original position: " << board.getFEN() << "\n";
    
    MoveGenerator gen;
    
    // Step 1: Make O-O
    Board::MoveState state1;
    board.makeMove("O-O", state1);
    std::cout << "2. After O-O: " << board.getFEN() << "\n";
    
    // Check that black has 43 moves
    uint64_t blackMoves = perft(board, gen, 1);
    std::cout << "   Black moves after O-O: " << blackMoves << " (expected: 43)\n";
    
    // Step 2: Make b4-b3
    Board::MoveState state2;
    board.makeMove("b4-b3", state2);
    std::cout << "3. After b4-b3: " << board.getFEN() << "\n";
    
    // Check that white has 49 moves
    uint64_t whiteMoves = perft(board, gen, 1);
    std::cout << "   White moves after b4-b3: " << whiteMoves << " (expected: 49)\n";
    
    std::cout << "\n=== COMPARISON WITH PERFT PATH ===\n";
    
    // Now test the same path using perft from original position
    Board origBoard;
    origBoard.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    // Make O-O
    Board::MoveState origState1;
    origBoard.makeMove("O-O", origState1);
    
    // Now count how many white moves result from b4-b3
    auto blackMovesFromCastled = gen.generateAllMoves(origBoard, false);
    
    for (auto move : blackMovesFromCastled) {
        std::string moveStr = decodeMove(move);
        if (moveStr == "b4-b3" && origBoard.isMoveLegal(moveStr)) {
            Board copy = origBoard;
            Board::MoveState copyState;
            copy.makeMove(moveStr, copyState);
            
            uint64_t resultingWhiteMoves = perft(copy, gen, 1);
            std::cout << "Via perft path - White moves after " << moveStr << ": " 
                     << resultingWhiteMoves << " (expected: 49)\n";
            
            if (resultingWhiteMoves != 49) {
                std::cout << "❌ DISCREPANCY FOUND!\n";
                std::cout << "Direct path gives: " << whiteMoves << "\n";
                std::cout << "Perft path gives: " << resultingWhiteMoves << "\n";
                std::cout << "Difference: " << (int64_t)resultingWhiteMoves - 49 << "\n";
            } else {
                std::cout << "✅ Both paths give correct result\n";
            }
            break;
        }
    }
    
    return 0;
}
