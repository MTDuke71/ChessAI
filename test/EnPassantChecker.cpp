#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>
#include <vector>
#include <string>
#include <set>

// Helper to check for en passant moves specifically
void checkEnPassantMoves(const Board& board, bool isWhite) {
    std::cout << "=== EN PASSANT CHECK ===\n";
    std::cout << "Side to move: " << (isWhite ? "White" : "Black") << "\n";
    std::cout << "En passant square: " << board.getEnPassantSquare() << "\n";
    
    MoveGenerator gen;
    auto moves = gen.generateAllMoves(board, isWhite);
    
    int enPassantCount = 0;
    for (auto move : moves) {
        if (moveSpecial(move) == 2) {  // En passant special flag
            enPassantCount++;
            std::cout << "En passant move: " << decodeMove(move) << "\n";
        }
    }
    
    std::cout << "Total en passant moves: " << enPassantCount << "\n\n";
}

// Detailed analysis of a specific problematic move sequence
void analyzeSequence() {
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== SEQUENCE ANALYSIS ===\n";
    
    // Test a few moves that might create en passant opportunities
    std::vector<std::string> testMoves = {"a2-a4", "g2-g4", "b2-b3"}; // Pawn moves that might enable en passant
    
    for (const std::string& move : testMoves) {
        std::cout << "\nAfter move: " << move << "\n";
        
        Board copy = board;
        Board::MoveState state;
        
        if (copy.isMoveLegal(move)) {
            copy.makeMove(move, state);
            checkEnPassantMoves(copy, false); // Black to move
            
            // Look for specific en passant responses
            MoveGenerator gen;
            auto blackMoves = gen.generateAllMoves(copy, false);
            
            for (auto blackMove : blackMoves) {
                if (moveSpecial(blackMove) == 2) {
                    std::string blackMoveStr = decodeMove(blackMove);
                    std::cout << "Found en passant response: " << blackMoveStr << "\n";
                    
                    // Make the en passant move and check white's responses
                    Board copy2 = copy;
                    Board::MoveState state2;
                    copy2.makeMove(blackMoveStr, state2);
                    
                    auto whiteMoves = gen.generateAllMoves(copy2, true);
                    std::cout << "White moves after en passant: " << whiteMoves.size() << "\n";
                }
            }
        }
    }
}

// Check for potential pawn promotion issues
void checkPromotions() {
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== PROMOTION CHECK ===\n";
    
    MoveGenerator gen;
    
    // Look through all possible 3-move sequences for promotion opportunities
    auto moves = gen.generateAllMoves(board, true);
    
    for (auto move : moves) {
        std::string moveStr = decodeMove(move);
        if (!board.isMoveLegal(moveStr)) continue;
        
        Board copy = board;
        Board::MoveState state;
        copy.makeMove(moveStr, state);
        
        auto blackMoves = gen.generateAllMoves(copy, false);
        for (auto blackMove : blackMoves) {
            std::string blackMoveStr = decodeMove(blackMove);
            if (!copy.isMoveLegal(blackMoveStr)) continue;
            
            Board copy2 = copy;
            Board::MoveState state2;
            copy2.makeMove(blackMoveStr, state2);
            
            auto whiteMoves = gen.generateAllMoves(copy2, true);
            for (auto whiteMove : whiteMoves) {
                if (moveSpecial(whiteMove) == 1) {  // Promotion
                    std::cout << "Promotion found in sequence: " << moveStr 
                              << " -> " << blackMoveStr 
                              << " -> " << decodeMove(whiteMove) << "\n";
                }
            }
        }
    }
}

int main() {
    analyzeSequence();
    checkPromotions();
    return 0;
}
