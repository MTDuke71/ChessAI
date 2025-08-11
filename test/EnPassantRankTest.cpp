#include <iostream>
#include "Board.h"
#include "FastMoveGenerator.h"

int main() {
    std::cout << "Testing En Passant Rank Calculation\n";
    std::cout << "===================================\n\n";
    
    // Test en passant rank masks
    uint64_t whiteEnPassantRank = 0x00FF000000000000ULL; // 5th rank (ranks are 0-indexed, so rank 4)
    uint64_t blackEnPassantRank = 0x0000000000FF0000ULL; // 4th rank (rank 3)
    
    std::cout << "White en passant rank (5th rank): 0x" << std::hex << whiteEnPassantRank << std::endl;
    std::cout << "Black en passant rank (4th rank): 0x" << std::hex << blackEnPassantRank << std::endl;
    
    // Test with actual position
    Board board;
    // Create position: e2-e4 d7-d5 e4-e5 f7-f5 (en passant available on f6)
    Board::MoveState s1, s2, s3, s4;
    board.makeMove("e2-e4", s1);
    board.makeMove("d7-d5", s2);
    board.makeMove("e4-e5", s3);
    board.makeMove("f7-f5", s4);
    
    std::cout << std::dec << "\nPosition after e2-e4 d7-d5 e4-e5 f7-f5:\n";
    std::cout << "FEN: " << board.getFEN() << "\n";
    std::cout << "En passant square: " << board.getEnPassantSquare() << "\n";
    
    FastMoveGenerator gen;
    FastMoveGenerator::MoveList moves;
    gen.generateLegalMoves(board, true, moves);
    
    std::cout << "White moves available: " << moves.count << "\n";
    
    // Look for en passant
    for (int i = 0; i < moves.count; ++i) {
        std::string move = moves.moves[i].toAlgebraic();
        if (move == "e5-f6") {
            std::cout << "Found en passant: " << move << "\n";
        }
    }
    
    return 0;
}
