// -----------------------------------------------------------------------------
// Minimal endgame tablebase for quick lookup of simple positions.
// -----------------------------------------------------------------------------
#include "Tablebase.h"

// -----------------------------------------------------------------------------
// Populates the internal table with a small set of known positions.
// -----------------------------------------------------------------------------
Tablebase::Tablebase() {
    // Minimal endgame tablebase entries
    tb["8/8/8/8/8/8/5k2/6K1 w - - 0 1"] = "Kg2"; // trivial draw
}

// -----------------------------------------------------------------------------
// Looks up a move for the given board if a tablebase entry exists.
// -----------------------------------------------------------------------------
std::optional<std::string> Tablebase::lookupMove(const Board& board) const {
    auto it = tb.find(board.getFEN());
    if (it != tb.end()) return it->second;
    return std::nullopt;
}
