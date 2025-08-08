#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

int main() {
    // Initial position
    std::string fen = "4k3/8/8/8/8/8/8/4K2R w K - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== Castling Rights Analysis ===\n";
    std::cout << "Initial position:\n";
    board.printBoard();
    std::cout << "FEN: " << board.getFEN() << "\n";
    std::cout << "White can castle kingside: " << (board.canCastleWK() ? "YES" : "NO") << "\n\n";
    
    // Make e1e2 move
    uint16_t e1e2Move = encodeMove("e1-e2");
    Board::MoveState state;
    board.makeMove(e1e2Move, state);
    
    std::cout << "After e1e2:\n";
    board.printBoard();
    std::cout << "FEN: " << board.getFEN() << "\n";
    std::cout << "White can castle kingside: " << (board.canCastleWK() ? "YES" : "NO") << "\n";
    std::cout << "Side to move: " << (board.isWhiteToMove() ? "White" : "Black") << "\n\n";
    
    // Check if there's any unexpected castling happening
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, false); // Black to move
    
    std::cout << "Black moves available: " << moves.size() << "\n";
    for (uint16_t move : moves) {
        std::string moveStr = decodeMove(move);
        int special = moveSpecial(move);
        if (special == 3) {
            std::cout << "CASTLING MOVE FOUND: " << moveStr << "\n";
        }
    }
    
    board.unmakeMove(state);
    return 0;
}
