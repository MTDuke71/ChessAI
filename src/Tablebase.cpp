#include "Tablebase.h"

Tablebase::Tablebase() {
    // Minimal endgame tablebase entries
    tb["8/8/8/8/8/8/5k2/6K1 w - - 0 1"] = "Kg2"; // trivial draw
}

std::optional<std::string> Tablebase::lookupMove(const Board& board) const {
    auto it = tb.find(board.getFEN());
    if (it != tb.end()) return it->second;
    return std::nullopt;
}
