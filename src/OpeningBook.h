#pragma once
#include "Board.h"
#include <unordered_map>
#include <string>
#include <optional>

class OpeningBook {
public:
    OpeningBook();
    std::optional<std::string> getBookMove(const Board& board) const;
private:
    std::unordered_map<std::string, std::string> book;
};
