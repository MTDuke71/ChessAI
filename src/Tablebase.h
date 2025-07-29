#pragma once
#include "Board.h"
#include <unordered_map>
#include <optional>
#include <string>

class Tablebase {
public:
    Tablebase();
    std::optional<std::string> lookupMove(const Board& board) const;
private:
    std::unordered_map<std::string, std::string> tb;
};
