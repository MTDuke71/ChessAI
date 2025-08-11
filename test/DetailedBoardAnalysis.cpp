#include <iostream>
#include "Board.h"
#include "FastMoveGenerator.h"
#include "MoveGenerator.h"
#include "MoveEncoding.h"

void printBoardState(const Board& board, const std::string& description) {
    std::cout << "\n=== " << description << " ===\n";
    std::cout << "En passant square: " << board.getEnPassantSquare() << "\n";
    std::cout << "White castling rights: K=" << board.canCastleWK() << " Q=" << board.canCastleWQ() << "\n";
    std::cout << "Black castling rights: K=" << board.canCastleBK() << " Q=" << board.canCastleBQ() << "\n";
}

int main() {
    std::cout << "Detailed Board State Analysis\n";
    std::cout << "=============================\n";
    
    Board board;
    printBoardState(board, "Starting position");
    
    // Move sequence: e2-e4 e7-e5 g1-f3 b8-c6
    std::vector<std::string> moves = {"e2-e4", "e7-e5", "g1-f3", "b8-c6"};
    std::vector<Board::MoveState> states;
    
    for (const auto& moveStr : moves) {
        Board::MoveState state;
        board.makeMove(moveStr, state);
        states.push_back(state);
        printBoardState(board, "After " + moveStr);
    }
    
    // Compare move generation in detail
    FastMoveGenerator fastGen;
    MoveGenerator originalGen;
    
    FastMoveGenerator::MoveList fastMoves;
    fastGen.generateLegalMoves(board, true, fastMoves);
    
    std::vector<uint16_t> originalMoves = originalGen.generateLegalMoves(board, true);
    
    std::cout << "\n=== Move Generation Comparison ===\n";
    std::cout << "FastMoveGenerator moves (" << fastMoves.count << "):\n";
    for (int i = 0; i < fastMoves.count; ++i) {
        std::string moveStr = fastMoves.moves[i].toAlgebraic();
        std::cout << "  " << moveStr;
        if (moveStr == "f2-f4") std::cout << " <-- EXTRA";
        std::cout << "\n";
    }
    
    std::cout << "\nOriginal MoveGenerator moves (" << originalMoves.size() << "):\n";
    for (const auto& move : originalMoves) {
        std::cout << "  " << decodeMove(move) << "\n";
    }
    
    // The original MoveGenerator doesn't have generatePseudoLegalMoves method
    // So let's focus on the legal move difference
    std::cout << "\n=== Analysis Summary ===\n";
    std::cout << "FastMoveGenerator has f2-f4, Original doesn't\n";
    std::cout << "This suggests FastMoveGenerator's legal filtering is too permissive\n";
    
    return 0;
}
