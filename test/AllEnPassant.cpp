#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

void checkAllEnPassantOpportunities() {
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== CHECKING ALL EN PASSANT OPPORTUNITIES ===\n";
    
    // Test all white pawn moves that could create en passant opportunities
    std::vector<std::string> whitePawnMoves = {"a2-a4", "b2-b4", "c2-c4", "f2-f4", "g2-g4", "h2-h4"};
    
    for (const std::string& move : whitePawnMoves) {
        if (!board.isMoveLegal(move)) continue;
        
        Board copy = board;
        Board::MoveState state;
        copy.makeMove(move, state);
        
        std::cout << "\nAfter " << move << ":\n";
        std::cout << "En passant square: " << copy.getEnPassantSquare() << "\n";
        
        // Check for en passant responses
        MoveGenerator gen;
        auto moves = gen.generateAllMoves(copy, false);
        
        int enPassantCount = 0;
        for (auto blackMove : moves) {
            if (moveSpecial(blackMove) == 2) {
                enPassantCount++;
                std::cout << "  En passant move: " << decodeMove(blackMove) << "\n";
            }
        }
        
        if (enPassantCount == 0) {
            std::cout << "  No en passant moves available\n";
        }
    }
    
    // Also check some black pawn moves in sequences
    std::cout << "\n=== CHECKING BLACK PAWN EN PASSANT SEQUENCES ===\n";
    
    // Test sequences where black makes a pawn move that could be captured en passant
    std::vector<std::pair<std::string, std::string>> sequences = {
        {"d5-d6", "c7-c5"},    // After d5-d6, black might play c7-c5
        {"e5-d7", "c7-c5"},    // After knight moves, black c7-c5
        {"f3-f5", "g6-g5"}     // After f3-f5, black might play g6-g5 
    };
    
    for (const auto& [whiteMove, blackMove] : sequences) {
        if (!board.isMoveLegal(whiteMove)) continue;
        
        Board copy = board;
        Board::MoveState state1;
        copy.makeMove(whiteMove, state1);
        
        if (!copy.isMoveLegal(blackMove)) continue;
        
        Board::MoveState state2;
        copy.makeMove(blackMove, state2);
        
        std::cout << "\nAfter " << whiteMove << " " << blackMove << ":\n";
        std::cout << "En passant square: " << copy.getEnPassantSquare() << "\n";
        
        // Check for white en passant responses
        MoveGenerator gen;
        auto moves = gen.generateAllMoves(copy, true);
        
        int enPassantCount = 0;
        for (auto whiteMove2 : moves) {
            if (moveSpecial(whiteMove2) == 2) {
                enPassantCount++;
                std::cout << "  White en passant move: " << decodeMove(whiteMove2) << "\n";
            }
        }
        
        if (enPassantCount == 0) {
            std::cout << "  No white en passant moves available\n";
        }
    }
}

int main() {
    checkAllEnPassantOpportunities();
    return 0;
}
