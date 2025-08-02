#pragma once
#include "Board.h"
#include "MoveGenerator.h"
#include <string>
#include <optional>
#include <unordered_map>
#include <cstdint>
#include <iosfwd>

class OpeningBook {
public:
    explicit OpeningBook(const std::string& file = "books/komodo.bin");
    std::optional<std::string> getBookMove(const Board& board) const;
    void print(std::ostream& out) const;
private:
    struct Entry { uint16_t move; uint16_t weight; uint32_t learn; };
    std::unordered_multimap<uint64_t, Entry> entries;
    static uint64_t polyglotHash(const Board& board);
    static std::string decodeMove(uint16_t move);
};
