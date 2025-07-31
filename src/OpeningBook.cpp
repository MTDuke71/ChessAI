#include "OpeningBook.h"

OpeningBook::OpeningBook() {
    // Very small built-in opening book
    book["rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"] = "e2-e4";
    book["rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1"] = "e7-e5";
    book["rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2"] = "g1-f3";
    book["rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq - 0 2"] = "d7-d5";
}

std::optional<std::string> OpeningBook::getBookMove(const Board& board) const {
    auto it = book.find(board.getFEN());
    if (it != book.end()) return it->second;
    return std::nullopt;
}
