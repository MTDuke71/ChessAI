#include "OpeningBook.h"

OpeningBook::OpeningBook() {
    // Very small built-in opening book
    book["rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"] = "e2-e4";
    book["rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1"] = "e7-e5";
}

std::optional<std::string> OpeningBook::getBookMove(const Board& board) const {
    auto it = book.find(board.getFEN());
    if (it != book.end()) return it->second;
    return std::nullopt;
}
