#include <iostream>
#include "Board.h"
#include "MoveGenerator.h"

int main() {
    std::cout << "Welcome to ChessAI!" << std::endl;

    Board board;
    board.printBoard();

    MoveGenerator moveGen;
    std::vector<uint32_t> pawnMoves = moveGen.generatePawnMoves(board, true);

    std::cout << "Pawn Moves (White): " << std::endl;
    if (pawnMoves.empty()) {
        std::cout << "No valid pawn moves found.\n";
    }
    else {
        for (auto move : pawnMoves) {
            int from = move & 0x3F;       // Extract source square
            int to = (move >> 6) & 0x3F; // Extract target square
            std::cout << "From: " << from << " To: " << to << std::endl;
        }
    }

    return 0;
}